//===- AttributeAdjuster.cpp ----------------------------------------------===//
//
//===----------------------------------------------------------------------===//
//
// This pass is used solely to remove attribute OptimizeNone from the list of
// function attributes, so that instruction combining can work.
//===----------------------------------------------------------------------===//

#include "TPCTargetMachine.h"

using namespace llvm;


namespace {
struct AttributeAdjuster {
  static bool runImpl(Function &F) {
    if (F.hasFnAttribute(Attribute::OptimizeNone)) {
      F.removeFnAttr(Attribute::OptimizeNone);
      return true;
    }
    return false;
  }
};
}

PreservedAnalyses AttributeAdjusterPass::run(Function &F,
                                             FunctionAnalysisManager &AM) {
  if (!AttributeAdjuster::runImpl(F))
    return PreservedAnalyses::all();
  return PreservedAnalyses::none();
}

