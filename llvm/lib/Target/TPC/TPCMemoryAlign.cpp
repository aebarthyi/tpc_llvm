//===- TPCMemoryAlign.cpp - Align vector accesses to the VLM row ----------===//
//
// Clang gives a load or store the alignment of its C type, and a bool64,
// bool128 or bool256 is 8, 16 or 32 bytes to C. Its IR type, <256 x i1>, is
// aligned to 256 bytes like every vector, and an array of them has a stride
// of 256, so `bool64 valid[4]; ... valid[k] = ...` stores with align 8 to an
// address that is always a multiple of 256. The backend cannot select a
// <256 x i1> access aligned to less than its type: it expands it through a
// stack slot and fails on the stack load ("Cannot select ... from %stack.0").
// From -O1 on, InferAlignment raises the alignment first; at -O0 nothing did
// (LLVM 14's InstCombine inferred alignment, LLVM 23's does not).
//
// Every vector access in private memory (address space 0, which the
// Globalizer later moves to local memory) and vector local memory (address
// space 2) is to a whole 256-byte row, so this pass gives each one the
// alignment of its type, at all optimization levels, as Habana's compiler
// does in its "TPC memory access aligner". Habana's pass also raises scalar
// accesses in address space 0 to 256; scalars compile at their own alignment,
// so they are left alone here.
//
// 1cat's deepseek_v41_qk_flat_reuse_softmax_gaudi2 failed at -O0 this way.
//
//===----------------------------------------------------------------------===//

#include "TPCTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

#define DEBUG_TYPE "tpcmemalign"

STATISTIC(NumAligned, "Vector loads and stores given their type's alignment");

// The alignment Ty's accesses get at address space AS, or nothing if they
// keep their own.
static MaybeAlign rowAlign(const DataLayout &DL, Type *Ty, unsigned AS) {
  if (AS != 0 && AS != 2)
    return std::nullopt;
  if (!isa<FixedVectorType>(Ty))
    return std::nullopt;
  Align A = DL.getABITypeAlign(Ty);
  if (A < Align(256))
    return std::nullopt;
  return A;
}

PreservedAnalyses TPCMemoryAlignPass::run(Function &F,
                                          FunctionAnalysisManager &) {
  const DataLayout &DL = F.getDataLayout();
  bool Changed = false;
  for (Instruction &I : instructions(F)) {
    if (auto *LI = dyn_cast<LoadInst>(&I)) {
      MaybeAlign A = rowAlign(DL, LI->getType(), LI->getPointerAddressSpace());
      if (A && LI->getAlign() < *A) {
        LI->setAlignment(*A);
        ++NumAligned;
        Changed = true;
      }
    } else if (auto *SI = dyn_cast<StoreInst>(&I)) {
      MaybeAlign A = rowAlign(DL, SI->getValueOperand()->getType(),
                              SI->getPointerAddressSpace());
      if (A && SI->getAlign() < *A) {
        SI->setAlignment(*A);
        ++NumAligned;
        Changed = true;
      }
    }
  }
  if (!Changed)
    return PreservedAnalyses::all();
  PreservedAnalyses PA;
  PA.preserveSet<CFGAnalyses>();
  return PA;
}
