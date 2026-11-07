#ifndef _TPC_TARGET_MACHINE_H_
#define _TPC_TARGET_MACHINE_H_

#include "TPCInstrInfo.h"
#include "TPCSubtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/PassManager.h"
#include <memory>

namespace llvm {

class LPMUpdater;
class Module;

LoopPass *createTpcLoopOptPass();
void initializeTpcLoopOptPass(PassRegistry&);

class TpcLoopOptImpl;

/// New PM version of TpcLoopOpt ("tpcopt"). Like the legacy pass, one pass
/// object keeps its transformation state across all loops it is run on.
class TpcLoopOptPass : public PassInfoMixin<TpcLoopOptPass> {
  std::unique_ptr<TpcLoopOptImpl> Impl;

public:
  TpcLoopOptPass();
  TpcLoopOptPass(TpcLoopOptPass &&);
  TpcLoopOptPass &operator=(TpcLoopOptPass &&);
  ~TpcLoopOptPass();

  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);
  // The legacy pass does not call skipLoop().
  static bool isRequired() { return true; }
};


FunctionPass *createScalarToIRFPass(bool FoldCmp);
void initializeScalarToIRFPassPass(PassRegistry &);

/// New PM version of ScalarToIRFPass ("scalar2irf").
class TPCScalarToIRFPass : public PassInfoMixin<TPCScalarToIRFPass> {
  bool FoldCmp;

public:
  explicit TPCScalarToIRFPass(bool FoldCmp = true) : FoldCmp(FoldCmp) {}
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

ModulePass *createGlobalResolver();
void initializeGlobalResolverPass(PassRegistry&);

/// New PM version of GlobalResolver ("glbresolver").
class GlobalResolverPass : public PassInfoMixin<GlobalResolverPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

ModulePass *createGlobalizer();
void initializeGlobalizerPass(PassRegistry&);

/// New PM version of Globalizer ("globalizer").
class GlobalizerPass : public PassInfoMixin<GlobalizerPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

FunctionPass *createTPCIndexMap();
void initializeTPCIndexMapPass(PassRegistry&);

FunctionPass *createTPCIndexGen();
void initializeTPCIndexGenPass(PassRegistry &);

FunctionPass *createTPCModuleLegalizer();
void initializeTPCModuleLegalizerPass(PassRegistry&);

/// New PM version of TPCModuleLegalizer ("tpclegal"). The legacy pass is a
/// FunctionPass that does all of its work in doInitialization(Module &), so
/// the new PM version is a module pass.
class TPCModuleLegalizerPass : public PassInfoMixin<TPCModuleLegalizerPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
  // The legacy pass never skips (it runs at all optimization levels).
  static bool isRequired() { return true; }
};

FunctionPass *createTpcCopyElision();
void initializeTpcCopyElisionPass(PassRegistry&);

/// New PM version of TpcCopyElision ("copyelide").
class TpcCopyElisionPass : public PassInfoMixin<TpcCopyElisionPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

FunctionPass *createAttributeAdjuster();
void initializeAttributeAdjusterPass(PassRegistry&);

/// New PM version of AttributeAdjuster ("adjust-attr").
class AttributeAdjusterPass : public PassInfoMixin<AttributeAdjusterPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
  // Must run on optnone functions: removing optnone is its whole purpose.
  static bool isRequired() { return true; }
};

FunctionPass *createPromoteMemoryToRegsPass();
void initializePromoteMemoryToRegsPass(PassRegistry&);

struct TPCEliminateRedundantLinearityPassConfig {
  bool HasFeatureTensorStorePack32To16 = false;
  bool HasFeatureTensorStorePack32To8 = false;
  bool HasFeatureTensorStorePack16To8 = false;
  bool HasFeatureTensorLoadUnpack = false;
};

FunctionPass *createTPCEliminateRedundantLinearityPass(
    TPCEliminateRedundantLinearityPassConfig Cfg);
void initializeEliminateRedundantLinearityLegacyPassPass(PassRegistry &);

/// New PM version of EliminateRedundantLinearityLegacyPass
/// ("tpc-eliminate-redundant-linearity").
class TPCEliminateRedundantLinearityPass
    : public PassInfoMixin<TPCEliminateRedundantLinearityPass> {
  TPCEliminateRedundantLinearityPassConfig Cfg;

public:
  explicit TPCEliminateRedundantLinearityPass(
      TPCEliminateRedundantLinearityPassConfig Cfg = {})
      : Cfg(Cfg) {}
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

FunctionPass *createMarkupDirectMMIOAccessPass();
void initializeMarkupDirectMMIOAccessPassPass(PassRegistry &);

/// New PM version of MarkupDirectMMIOAccessPass
/// ("tpc-markup-direct-mmio-access").
class TPCMarkupDirectMMIOAccessPass
    : public PassInfoMixin<TPCMarkupDirectMMIOAccessPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

FunctionPass *createTPCPredicateOptimizer();
void initializeTPCPredicateOptimizerPass(PassRegistry &);

FunctionPass *createTPCIncMerger();
void initializeTPCIncMergerPass(PassRegistry &);

FunctionPass *createTPCTransformIntrinPass();

/// New PM version of TPCTransformIntrin ("tpc-trans-intr").
class TPCTransformIntrinPass : public PassInfoMixin<TPCTransformIntrinPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

FunctionPass *createTPCHardwareLoops();
FunctionPass *createTPCPipeliner();
FunctionPass *createTPCPacketizer();
FunctionPass *createTPCExpandHWRegCopies();
FunctionPass *createTPCUnHardwareLoops();
FunctionPass *createTPCSetSpillBase();
FunctionPass *createTPCSetIndxCoalescer();
#ifdef DEPRECATED_TPC_ADDR_OPT
FunctionPass *createTPCAddrOpt();
#endif
FunctionPass *createTPCHWWA2();
FunctionPass *createTPCRegisterBalancer();
FunctionPass *createTPCRegisterCounter();
FunctionPass *createTPCElfSpecSet();
FunctionPass *createTPCLutCacheCounter();
FunctionPass *createTPCLatencyResolver();
FunctionPass *createTPCCostModelEmitter();
FunctionPass *createEventProfilerInject();
FunctionPass *createTPCFMAoptPass();
FunctionPass *createTPCUnbranchPass();
FunctionPass *createTPCSelectorPreshaper();
FunctionPass *createTPCSingleUseScalarOptimizer();
FunctionPass *createTPCSubregInitElimination();
FunctionPass *createTPCPipelineRegs();
FunctionPass *createTPCInstrCompress();
FunctionPass *createTPCMemorySize();
FunctionPass *createTPCPreRAHwWorkarounds();
FunctionPass *createTPCHWWAGeneral();
FunctionPass *createTPCMovCoalescer();
FunctionPass *createTPCImmToReg();
FunctionPass *createTPCScalarSink();
FunctionPass *createTPCSwapFakePredicate();

class TPCTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  TPCSubtarget *Subtarget = nullptr;
  mutable StringMap<std::unique_ptr<TPCSubtarget>> SubtargetMap;

  TPCSubtarget *createSubtarget(StringRef CPU, StringRef FS) const;

public:
  TPCTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                   CodeGenOpt::Level OL, bool JIT);

  ~TPCTargetMachine() override;

  const TPCSubtarget *getSubtarget() const { return Subtarget; }
  const TPCSubtarget *getSubtargetImpl(const Function &F) const override;

  void adjustPassManager(PassManagerBuilder &) override;
  void registerPassBuilderCallbacks(PassBuilder &PB) override;
  void registerDefaultAliasAnalyses(AAManager &AAM) override;
  /// Subtarget features used by TPCEliminateRedundantLinearity.
  TPCEliminateRedundantLinearityPassConfig
  getEliminateRedundantLinearityConfig() const;
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;

 bool usesPhysRegsForValues() const override { return false; }
};

} // end namespace llvm

#endif
