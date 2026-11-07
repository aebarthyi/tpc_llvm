//===---------------------------- EliminateSwizzleCast.h ------------------===//
//
//
//                     The LLVM Compiler Infrastructure:
//
//              2020 - This pass is a property of Habana labs
//
// Author : Md Shahid
// Email  : aashahid@habana.ai
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_CASTSWIZZLE_H
#define LLVM_TRANSFORMS_SCALAR_CASTSWIZZLE_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class FunctionPass;

class EliminateSwizzleCastPass
    : public PassInfoMixin<EliminateSwizzleCastPass> {
public:
  explicit EliminateSwizzleCastPass() {}

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // end namespace llvm

#endif
