//===- TPCRemainderLoops.cpp - No runtime unroll of a short loop ----------===//
//
// TPCTTIImpl::getUnrollingPreferences has LLVM's unroller unroll a loop that
// carries a sum or another value at run time, with a remainder loop, which
// costs a trip count computation each time the loop is entered. LLVM does not
// unroll so a loop that runs at most 8 times (-unroll-max-upperbound) when
// SCEV knows it does, but SCEV does not know it of a loop that finishes the
// keys a loop with a larger step left,
//
//   for (; i + 512 <= n; i += 512) ...
//   for (; i < n; i += 64) ...
//
// which starts with n - i < 512 whether the first loop ran or not, so runs at
// most 8 times: its start is a phi of the two ways in, of which SCEV knows
// nothing. Such a loop is given llvm.loop.unroll.runtime.disable here, before
// the unroller takes the first loop and the exit test that shows it.
//
// wright's qsa_select enters two such loops, after its 512-key loop, in each
// of the 32 steps of a binary search; with their trip count and remainder
// computations it took 1.37 times Habana's cycles in the simulator, and 1.09
// without.
//
//===----------------------------------------------------------------------===//

#include "TPCTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/IVDescriptors.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/LoopUtils.h"

using namespace llvm;

#define DEBUG_TYPE "tpc-remainder-loops"

STATISTIC(NumLoops, "Short loops kept from the runtime unroller");

static cl::opt<unsigned> MaxTrips(
    "tpc-remainder-loop-trips", cl::Hidden, cl::init(8),
    cl::desc("Keep a loop from the runtime unroller when the branches that "
             "lead to it show it runs at most this many times (0: never)"));

namespace {
// Replaces one expression by another in a SCEV. Of the operands of a min or
// max that then differ by a constant, it keeps the one the min or max takes
// if nothing wraps, which SCEV does not assume: the start replaced by
// `n - 255` turns `smax(i + 64, n)` into `smax(n - 191, n)`.
class SCEVReplacer : public SCEVRewriteVisitor<SCEVReplacer> {
  const SCEV *From, *To;

  SmallVector<SCEVUse, 4> fold(const SCEVNAryExpr *E, bool Max) {
    SmallVector<SCEVUse, 4> Ops;
    for (const SCEV *Op : E->operands()) {
      const SCEV *New = visit(Op);
      bool Merged = false;
      for (SCEVUse &Old : Ops)
        if (auto *D = dyn_cast<SCEVConstant>(SE.getMinusSCEV(New, Old))) {
          if (D->getAPInt().isNegative() != Max)
            Old = New;
          Merged = true;
          break;
        }
      if (!Merged)
        Ops.push_back(New);
    }
    return Ops;
  }

public:
  SCEVReplacer(ScalarEvolution &SE, const SCEV *From, const SCEV *To)
      : SCEVRewriteVisitor(SE), From(From), To(To) {}
  const SCEV *visit(const SCEV *S) {
    return S == From ? To : SCEVRewriteVisitor::visit(S);
  }
  const SCEV *visitSMaxExpr(const SCEVSMaxExpr *E) {
    auto Ops = fold(E, true);
    return SE.getSMaxExpr(Ops);
  }
  const SCEV *visitUMaxExpr(const SCEVUMaxExpr *E) {
    auto Ops = fold(E, true);
    return SE.getUMaxExpr(Ops);
  }
  const SCEV *visitSMinExpr(const SCEVSMinExpr *E) {
    auto Ops = fold(E, false);
    return SE.getSMinExpr(Ops);
  }
  const SCEV *visitUMinExpr(const SCEVUMinExpr *E) {
    auto Ops = fold(E, false);
    return SE.getUMinExpr(Ops);
  }
};
} // namespace

// The most times L runs, given a branch condition `x < v + c` (or `<=`) on
// each way into it, where v is the start of a counting-up induction variable
// on that way and c a constant: the start is then at least x - c + 1, and the
// backedge-taken count from there is the most for that way. Where the way
// comes out of an earlier loop, v is also that loop's induction variable as
// its exit test sees it in the last iteration. 0: no bound for some way in.
static uint64_t maxTripCountFromGuards(const Loop &L, ScalarEvolution &SE,
                                       LoopInfo &LI) {
  BasicBlock *Preheader = L.getLoopPreheader();
  const SCEV *BTC = SE.getBackedgeTakenCount(&L);
  if (!Preheader || isa<SCEVCouldNotCompute>(BTC))
    return 0;
  for (PHINode &Phi : L.getHeader()->phis()) {
    InductionDescriptor ID;
    if (!Phi.getType()->isIntegerTy() ||
        !InductionDescriptor::isInductionPHI(&Phi, &L, &SE, ID))
      continue;
    const ConstantInt *Step = ID.getConstIntStepValue();
    if (!Step || !Step->getValue().isStrictlyPositive())
      continue;
    Value *Start = Phi.getIncomingValueForBlock(Preheader);
    const SCEV *StartS = SE.getSCEV(Start);
    if (StartS->getType() != BTC->getType())
      continue;
    // Each way in: the start's value on it and the edge it comes by.
    SmallVector<std::tuple<Value *, BasicBlock *, BasicBlock *>, 4> Ways;
    if (auto *SP = dyn_cast<PHINode>(Start))
      for (unsigned I = 0, E = SP->getNumIncomingValues(); I < E; ++I)
        Ways.emplace_back(SP->getIncomingValue(I), SP->getIncomingBlock(I),
                          SP->getParent());
    else
      Ways.emplace_back(Start, Preheader, L.getHeader());
    uint64_t Max = 0;
    bool Bounded = true;
    for (auto [V, From, To] : Ways) {
      const SCEV *VS = SE.getSCEV(V);
      // What v is, as the conditions on the way see it.
      SmallVector<const SCEV *, 4> Views = {VS};
      if (auto *P = dyn_cast<PHINode>(V); P && P->getNumIncomingValues() == 1)
        Views.push_back(SE.getSCEV(P->getIncomingValue(0)));
      std::optional<uint64_t> Most;
      // Up the edges that every path to this way in takes.
      for (unsigned Depth = 0; From && Depth < 4 && !Most; ++Depth) {
        // Out of an earlier loop: v may be the exit value of one of its
        // induction variables (as IndVarSimplify computes it).
        if (const Loop *A = LI.getLoopFor(From); A && !A->contains(To))
          for (PHINode &P : A->getHeader()->phis()) {
            if (P.getType() != Phi.getType() || !A->getLoopLatch())
              continue;
            for (Value *IV :
                 {(Value *)&P, P.getIncomingValueForBlock(A->getLoopLatch())})
              if (auto *R = dyn_cast<SCEVAddRecExpr>(SE.getSCEV(IV));
                  R && R->getLoop() == A &&
                  SE.getSCEVAtScope(R, A->getParentLoop()) == VS)
                Views.push_back(R);
          }
        auto *Br = dyn_cast<CondBrInst>(From->getTerminator());
        auto *Cmp = Br ? dyn_cast<ICmpInst>(Br->getCondition()) : nullptr;
        if (Cmp && Br->getSuccessor(0) != Br->getSuccessor(1)) {
          CmpInst::Predicate Pred = Cmp->getPredicate();
          if (Br->getSuccessor(1) == To)
            Pred = CmpInst::getInversePredicate(Pred);
          Value *X = Cmp->getOperand(0), *Y = Cmp->getOperand(1);
          if (ICmpInst::isGT(Pred) || ICmpInst::isGE(Pred)) {
            std::swap(X, Y);
            Pred = CmpInst::getSwappedPredicate(Pred);
          }
          const SCEV *XS = SE.getSCEV(X), *YS = SE.getSCEV(Y);
          if ((ICmpInst::isLT(Pred) || ICmpInst::isLE(Pred)) &&
              XS->getType() == VS->getType() && YS->getType() == VS->getType())
            for (const SCEV *W : Views)
              if (auto *C = dyn_cast<SCEVConstant>(SE.getMinusSCEV(YS, W))) {
                // x < w + c: v = w > x - c.
                const SCEV *Least = SE.getMinusSCEV(XS, C);
                if (ICmpInst::isLT(Pred))
                  Least = SE.getAddExpr(Least, SE.getOne(Least->getType()));
                if (auto *K = dyn_cast<SCEVConstant>(
                        SCEVReplacer(SE, StartS, Least).visit(BTC));
                    K && K->getAPInt().getActiveBits() < 32) {
                  Most = K->getAPInt().getZExtValue() + 1;
                  break;
                }
              }
        }
        To = From;
        From = From->getSinglePredecessor();
      }
      if (!Most) {
        Bounded = false;
        break;
      }
      Max = std::max(Max, *Most);
    }
    if (Bounded)
      return Max;
  }
  return 0;
}

PreservedAnalyses TPCRemainderLoopPass::run(Loop &L, LoopAnalysisManager &,
                                            LoopStandardAnalysisResults &AR,
                                            LPMUpdater &) {
  if (!MaxTrips || !L.isInnermost())
    return PreservedAnalyses::all();
  // An unroll pragma decides for itself.
  if (MDNode *LoopID = L.getLoopID())
    for (const MDOperand &Op : drop_begin(LoopID->operands()))
      if (auto *MD = dyn_cast<MDNode>(Op.get()))
        if (auto *S = dyn_cast_or_null<MDString>(
                MD->getNumOperands() ? MD->getOperand(0).get() : nullptr);
            S && (S->getString().starts_with("llvm.loop.unroll.") ||
                  S->getString().starts_with("llvm.loop.machine.unroll.")))
          return PreservedAnalyses::all();
  uint64_t Trips = maxTripCountFromGuards(L, AR.SE, AR.LI);
  if (!Trips || Trips > MaxTrips)
    return PreservedAnalyses::all();
  LLVM_DEBUG(dbgs() << "Runs at most " << Trips << " times: " << L);
  LLVMContext &Ctx = L.getHeader()->getContext();
  SmallVector<Metadata *, 4> MDs = {nullptr};
  if (MDNode *LoopID = L.getLoopID())
    append_range(MDs, drop_begin(LoopID->operands()));
  MDs.push_back(
      MDNode::get(Ctx, MDString::get(Ctx, "llvm.loop.unroll.runtime.disable")));
  MDNode *NewLoopID = MDNode::getDistinct(Ctx, MDs);
  NewLoopID->replaceOperandWith(0, NewLoopID);
  L.setLoopID(NewLoopID);
  ++NumLoops;
  // Only the loop's metadata changed.
  return PreservedAnalyses::all();
}
