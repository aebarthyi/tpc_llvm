//===- TPCSelectIntoFOp.cpp - Move a select into a float operand ----------===//
//
// A sum added to only when a condition holds,
//
//   if (row == token) squares += x * x;
//
// becomes `s' = select c, s + m, s` once SimplifyCFG speculates the add, and
// the select then lies on the sum's chain: each add waits for the previous
// add and then its select (a predicated move, plus a copy back where the two
// do not get one register). InstCombine moves the select into the add's
// other operand, `s + select c, m, -0.0`, off the chain, but only where s is
// known not to be a NaN, since a signaling NaN would come out of the add
// quiet. Habana's compiler (LLVM 14) moves it regardless, and it is the
// reference: where the condition fails the result is then s + -0.0, which
// is s but for a NaN's payload (and a denormal, if the add flushes them).
// Done here after each InstCombine, for fadd, fsub and fmul, as InstCombine
// would.
//
// 1cat's deepseek_v41_mhc_control_reuse adds the squares of 6 tokens this
// way in each step of its 160-step loop; it took 1.20 times Habana's cycles
// in the simulator with the selects on the chain, and 1.11 without.
//
//===----------------------------------------------------------------------===//

#include "TPCTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

#define DEBUG_TYPE "tpc-select-into-fop"

STATISTIC(NumMoved, "Selects of a float operation moved into its operand");

static cl::opt<bool> EnableSelectIntoFOp(
    "tpc-enable-select-into-fop", cl::Hidden, cl::init(true),
    cl::desc("Turn `select c, s op m, s` into `s op select c, m, identity` "
             "for float add, sub and mul, whether or not s may be a NaN"));

// `select c, s op m, s` (or with the arms swapped) as `s op select c, m, id`.
static Instruction *moveIntoOperand(SelectInst &SI) {
  Value *Cond = SI.getCondition();
  for (bool Swapped : {false, true}) {
    Value *Arm = Swapped ? SI.getFalseValue() : SI.getTrueValue();
    Value *S = Swapped ? SI.getTrueValue() : SI.getFalseValue();
    auto *BO = dyn_cast<BinaryOperator>(Arm);
    if (!BO || !BO->hasOneUse())
      continue;
    const Instruction::BinaryOps Opc = BO->getOpcode();
    if (Opc != Instruction::FAdd && Opc != Instruction::FSub &&
        Opc != Instruction::FMul)
      continue;
    Value *M = nullptr;
    if (BO->getOperand(0) == S)
      M = BO->getOperand(1);
    else if (Opc != Instruction::FSub && BO->getOperand(1) == S)
      M = BO->getOperand(0);
    // A select of two constants gains nothing.
    if (!M || isa<Constant>(M))
      continue;
    Constant *Id = ConstantExpr::getBinOpIdentity(Opc, BO->getType(),
                                                  /*AllowRHSConstant=*/true);
    IRBuilder<> B(&SI);
    Value *NewSel =
        B.CreateSelect(Cond, Swapped ? Id : M, Swapped ? M : Id, BO->getName());
    auto *New = BinaryOperator::Create(Opc, S, NewSel, "", SI.getIterator());
    New->copyIRFlags(BO);
    // s itself is the result where the select took it, which the select's
    // flags speak for and the operation's do not.
    FastMathFlags SelFMF = cast<FPMathOperator>(SI).getFastMathFlags();
    New->setHasNoNaNs(BO->hasNoNaNs() && SelFMF.noNaNs());
    New->setHasNoInfs(BO->hasNoInfs() && SelFMF.noInfs());
    New->setHasNoSignedZeros(BO->hasNoSignedZeros() && SelFMF.noSignedZeros());
    New->setDebugLoc(SI.getDebugLoc());
    New->takeName(&SI);
    return New;
  }
  return nullptr;
}

PreservedAnalyses TPCSelectIntoFOpPass::run(Function &F,
                                            FunctionAnalysisManager &) {
  if (!EnableSelectIntoFOp)
    return PreservedAnalyses::all();
  bool Changed = false;
  for (Instruction &I : make_early_inc_range(instructions(F))) {
    auto *SI = dyn_cast<SelectInst>(&I);
    if (!SI || !SI->getType()->isFPOrFPVectorTy())
      continue;
    if (Instruction *New = moveIntoOperand(*SI)) {
      auto *BO = cast<Instruction>(SI->getTrueValue() == New->getOperand(0)
                                       ? SI->getFalseValue()
                                       : SI->getTrueValue());
      SI->replaceAllUsesWith(New);
      SI->eraseFromParent();
      BO->eraseFromParent();
      ++NumMoved;
      Changed = true;
    }
  }
  if (!Changed)
    return PreservedAnalyses::all();
  PreservedAnalyses PA;
  PA.preserveSet<CFGAnalyses>();
  return PA;
}
