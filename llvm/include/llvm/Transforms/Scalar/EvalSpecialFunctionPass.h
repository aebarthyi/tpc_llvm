#ifndef LLVM_TRANSFORMS_SCALAR_EVALSPECIALFUNCTIONPASS_H
#define LLVM_TRANSFORMS_SCALAR_EVALSPECIALFUNCTIONPASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class Module;

/// Expands TPC special functions (sin, cos, exp, log, sqrt, rsqrt, tanh,
/// reciprocal on <64 x float> and <128 x bfloat>) and vector fdiv into TPC
/// intrinsic sequences.
class EvalSpecialFunctionPass : public PassInfoMixin<EvalSpecialFunctionPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

} // end namespace llvm

#endif
