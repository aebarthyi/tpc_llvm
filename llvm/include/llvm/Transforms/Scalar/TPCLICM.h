//===------------------------------ TPCLICM.h -----------------------------===//
//
//
//                     The LLVM Compiler Infrastructure:
//
//              2020 - This pass is a property of Habana labs
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_TPCLICM_H
#define LLVM_TRANSFORMS_SCALAR_TPCLICM_H

#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"

namespace llvm {

class Loop;

/// Hoists TPC lookup intrinsics with constant operands (except the first one,
/// which must be computed from llvm.read_register) out of a loop, together
/// with the instructions computing their first operand.
class TPCLICMPass : public PassInfoMixin<TPCLICMPass> {
public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);
  // Like the legacy pass, which never calls skipLoop(), run on optnone
  // functions too.
  static bool isRequired() { return true; }
};

} // end namespace llvm

#endif // LLVM_TRANSFORMS_SCALAR_TPCLICM_H
