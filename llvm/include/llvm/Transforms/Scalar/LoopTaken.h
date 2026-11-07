//===- LoopTaken.h - Honor loop_taken pragma --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides the new pass manager interface of the pass that honors
// the loop_taken pragma. For every loop whose latch carries "llvm.loop.taken"
// metadata it inserts an llvm.assume guaranteeing that the loop is entered.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_LOOPTAKEN_H
#define LLVM_TRANSFORMS_SCALAR_LOOPTAKEN_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class Function;

class LoopTakenPass : public PassInfoMixin<LoopTakenPass> {
public:
  explicit LoopTakenPass(int OptLevel = 2) : OptLevel(OptLevel) {}

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

  /// The optimization level the pass was created with. It is currently unused,
  /// like the argument of the legacy createLoopTakenPass().
  int getOptLevel() const { return OptLevel; }

private:
  int OptLevel;
};

} // end namespace llvm

#endif // LLVM_TRANSFORMS_SCALAR_LOOPTAKEN_H
