#ifndef LLVM_TRANSFORMS_SCALAR_ITERATIONINTERLEAVE_H
#define LLVM_TRANSFORMS_SCALAR_ITERATIONINTERLEAVE_H

#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"

namespace llvm {

class Loop;

/// Reorders the instructions of an unrolled, single-block innermost loop so
/// that instructions of the different unrolled iterations are interleaved.
class IterationInterleavePass : public PassInfoMixin<IterationInterleavePass> {
public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);
  // Like the legacy pass, which never calls skipLoop(), run on optnone
  // functions too.
  static bool isRequired() { return true; }
};

} // end namespace llvm

#endif // LLVM_TRANSFORMS_SCALAR_ITERATIONINTERLEAVE_H
