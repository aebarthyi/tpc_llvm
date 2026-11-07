// Add metadata to store / load instructions with MMIO.

#include "MCTargetDesc/InstructionDB.h"
#include "TPCTargetMachine.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IntrinsicsTPC.h"

using namespace llvm;

static bool isDirectMMIOAccess(const Instruction &I) {
  const IntrinsicInst *const II = dyn_cast<const IntrinsicInst>(&I);
  if (!II)
    return false;

  const auto IsMMIOSwitch = [](const Value &V) {
    const ConstantInt *const CI = dyn_cast<const ConstantInt>(&V);
    return CI && (CI->getSExtValue() & TPCII::SW_MMIO);
  };

  return (II->getIntrinsicID() == Intrinsic::tpc_st_l &&
          IsMMIOSwitch(*II->getOperand(2))) ||
         (II->getIntrinsicID() == Intrinsic::tpc_ld_l &&
          IsMMIOSwitch(*II->getOperand(1)));
}

static void addDirectMMIOAccessMetadata(Function &F) {
  LLVMContext &C = F.getContext();
  MDNode *N = MDNode::get(C, MDString::get(C, "direct mmio access"));
  F.setMetadata("tpc.direct.mmio.access", N);
}

class MarkupDirectMMIOAccessPass {
public:
  static bool runImpl(Function &F) {
    const bool HasMMIOAccess = any_of(
        F, [](BasicBlock &BB) { return any_of(BB, isDirectMMIOAccess); });

    if (HasMMIOAccess)
      addDirectMMIOAccessMetadata(F);

    return HasMMIOAccess;
  }
};

namespace llvm {

PreservedAnalyses
TPCMarkupDirectMMIOAccessPass::run(Function &F, FunctionAnalysisManager &AM) {
  if (!MarkupDirectMMIOAccessPass::runImpl(F))
    return PreservedAnalyses::all();
  return PreservedAnalyses::none();
}

} // namespace llvm
