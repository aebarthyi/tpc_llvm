//===---------------------------- LoopSWPPass.h ---------------------------===//
//
//
//                     The LLVM Compiler Infrastructure:
//
//              2020 - This pass is a property of Habana labs
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_LOOPSWPPASS_H
#define LLVM_TRANSFORMS_SCALAR_LOOPSWPPASS_H

#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include <memory>

namespace llvm {

class Loop;
class LoopSWPImpl;

/// Software pipelining of unrolled, single-block innermost TPC loops at the IR
/// level: the loads of the next iteration(s) are issued ahead, starting in the
/// preheader, and the last iteration's computes/stores are peeled into the
/// exit block.
class LoopSWPPass : public PassInfoMixin<LoopSWPPass> {
public:
  explicit LoopSWPPass();
  LoopSWPPass(LoopSWPPass &&);
  LoopSWPPass &operator=(LoopSWPPass &&);
  ~LoopSWPPass();

  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);
  // Like the legacy pass, which never calls skipLoop(), run on optnone
  // functions too.
  static bool isRequired() { return true; }

private:
  // Like the legacy pass object, the implementation keeps part of its state
  // across loops, so it must live as long as this pass object.
  std::unique_ptr<LoopSWPImpl> Impl;
};

} // end namespace llvm

#endif
