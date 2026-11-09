//===- MemoryToReg.cpp --- Move values from memory to temporaries ---------===//
//
//===----------------------------------------------------------------------===//
//
// This is limited version of mem2reg pass. It is necessary for -O0 mode,
// as values that keep global pointers cannot be stored in memory.
//
//===----------------------------------------------------------------------===//

#include "TPCTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Utils/PromoteMemToReg.h"
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "tpcmem2reg"

STATISTIC(NumPromoted, "Number of alloca's promoted (TPC)");

static bool shouldMoveToReg(AllocaInst *AI) {
  if (auto *PT = dyn_cast<PointerType>(AI->getAllocatedType()))
    if (PT->getAddressSpace() == 3)
      return true;
  return false;
}

static bool promoteMemoryToRegister(Function &F, DominatorTree &DT,
                                    AssumptionCache &AC) {
  std::vector<AllocaInst *> Allocas;
  BasicBlock &BB = F.getEntryBlock(); // Get the entry node for the function
  bool Changed = false;

  while (true) {
    Allocas.clear();

    // Find allocas that are safe to promote, by looking at all instructions in
    // the entry node
    for (BasicBlock::iterator I = BB.begin(), E = --BB.end(); I != E; ++I)
      if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) // Is it an alloca?
        if (isAllocaPromotable(AI) && shouldMoveToReg(AI))
          Allocas.push_back(AI);

    if (Allocas.empty())
      break;

    PromoteMemToReg(Allocas, DT, &AC);
    NumPromoted += Allocas.size();
    Changed = true;
  }
  return Changed;
}

PreservedAnalyses TPCMemoryToRegPass::run(Function &F,
                                          FunctionAnalysisManager &AM) {
  auto &DT = AM.getResult<DominatorTreeAnalysis>(F);
  auto &AC = AM.getResult<AssumptionAnalysis>(F);
  if (!promoteMemoryToRegister(F, DT, AC))
    return PreservedAnalyses::all();
  PreservedAnalyses PA;
  PA.preserveSet<CFGAnalyses>();
  return PA;
}
