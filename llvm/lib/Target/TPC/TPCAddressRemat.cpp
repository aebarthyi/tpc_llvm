//===- TPCAddressRemat.cpp - Keep the live addresses within the registers -===//
//
// An address register also holds the tensor its gen_addr computed it for, and
// no spill can save that (TPCInstrInfo::storeRegToStackSlot reports an error
// instead): register allocation fails when more addresses are live at once
// than there are address registers (8 on gaudi2). The passes before it do not
// count them. LICM and machine LICM move a gen_addr whose coordinates do not
// change in a loop out of it, and the address then stays live across the
// loop: a loop of 6 loading through 2 addresses, fully unrolled, kept 12 live
// across the loop around it (RC99/pragma/pragma-loop-unroll-addresses.c
// without the pragma unroller's limit). Habana's compiler spills them through
// Z30, which loses the tensor unless an address goes back into the register it
// came from.
//
// This pass runs last before register allocation, in SSA form. While more
// addresses are live at some point than there are registers, it takes, of the
// addresses live there that an unpredicated gen_addr of an immediate tensor
// computes, the one used furthest ahead (of equals, the one whose uses are in
// the fewest loops), and computes it again right before each of its uses, from
// the same coordinates, deleting its gen_addr. The coordinates are in an index
// register, which can be spilled.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TPCMCTargetDesc.h"
#include "TPCInstrInfo.h"
#include "TPCSubtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "tpc-address-remat"

STATISTIC(NumAddresses, "Addresses computed again at their uses");
STATISTIC(NumGenAddrs, "gen_addr instructions added for them");

static cl::opt<unsigned> LiveAddresses(
    "tpc-live-addresses", cl::Hidden, cl::init(0),
    cl::desc("Most addresses live at once before register allocation "
             "(0: as many as there are address registers)"));

namespace llvm {
FunctionPass *createTPCAddressRemat();
void initializeTPCAddressRematPass(PassRegistry &);
} // namespace llvm

namespace {

// GEN_ADDR_ld/_st operands.
enum { OpDest, OpTensor, OpCoords, OpSwitches, OpIncome, OpPred, OpPolarity };

class TPCAddressRemat : public MachineFunctionPass {
public:
  static char ID;
  TPCAddressRemat() : MachineFunctionPass(ID) {
    initializeTPCAddressRematPass(*PassRegistry::getPassRegistry());
  }
  StringRef getPassName() const override {
    return "TPC live addresses within the address registers";
  }
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<MachineLoopInfoWrapperPass>();
    AU.addPreserved<MachineLoopInfoWrapperPass>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }
  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  MachineRegisterInfo *MRI = nullptr;
  const TargetInstrInfo *TII = nullptr;
  MachineLoopInfo *MLI = nullptr;

  /// The addresses: address registers with a value (a gen_addr's income is
  /// usually an IMPLICIT_DEF or a copy of one).
  SmallVector<Register, 32> Addrs;
  DenseMap<Register, unsigned> AddrIdx;
  /// Addresses this pass computed right before their use.
  DenseSet<Register> Recomputed;

  /// The most addresses live at one point, which addresses, and the point:
  /// right before MaxPos in MaxBlock.
  unsigned MaxLive = 0;
  BitVector MaxSet;
  MachineBasicBlock *MaxBlock = nullptr;
  MachineBasicBlock::iterator MaxPos;

  bool collect(MachineFunction &MF);
  void measure(MachineFunction &MF);
  bool isRecomputable(Register R) const;
  unsigned nextUse(Register R) const;
  uint64_t cost(Register R) const;
  void recompute(MachineFunction &MF, Register R);

  int addressIndex(const MachineOperand &MO) const {
    if (!MO.isReg() || !MO.getReg().isVirtual() || (MO.isUse() && MO.isUndef()))
      return -1;
    auto It = AddrIdx.find(MO.getReg());
    return It == AddrIdx.end() ? -1 : int(It->second);
  }
};

} // namespace

char TPCAddressRemat::ID = 0;

INITIALIZE_PASS_BEGIN(TPCAddressRemat, DEBUG_TYPE,
                      "TPC live addresses within the address registers", false,
                      false)
INITIALIZE_PASS_DEPENDENCY(MachineLoopInfoWrapperPass)
INITIALIZE_PASS_END(TPCAddressRemat, DEBUG_TYPE,
                    "TPC live addresses within the address registers", false,
                    false)

FunctionPass *llvm::createTPCAddressRemat() { return new TPCAddressRemat(); }

/// Whether the register holds no value: an IMPLICIT_DEF or a copy of one,
/// which register allocation (ProcessImplicitDefs) gives no register.
static bool isUndefined(const MachineRegisterInfo &MRI, Register R) {
  for (unsigned Depth = 0; Depth < 16; ++Depth) {
    const MachineInstr *Def = MRI.getUniqueVRegDef(R);
    if (!Def)
      return false;
    if (Def->isImplicitDef())
      return true;
    if (!Def->isCopy())
      return false;
    const MachineOperand &Src = Def->getOperand(1);
    if (Src.isUndef())
      return true;
    if (!Src.getReg().isVirtual())
      return false;
    R = Src.getReg();
  }
  return false;
}

/// Collects the addresses; false when a register has more than one
/// definition (not SSA).
bool TPCAddressRemat::collect(MachineFunction &MF) {
  Addrs.clear();
  AddrIdx.clear();
  for (unsigned I = 0, E = MRI->getNumVirtRegs(); I != E; ++I) {
    const Register R = Register::index2VirtReg(I);
    if (MRI->def_empty(R) ||
        !TPC::ADRFRegClass.hasSubClassEq(MRI->getRegClass(R)))
      continue;
    if (!MRI->getUniqueVRegDef(R))
      return false;
    if (isUndefined(*MRI, R))
      continue;
    AddrIdx[R] = Addrs.size();
    Addrs.push_back(R);
  }
  return true;
}

/// Finds the most addresses live at one point.
void TPCAddressRemat::measure(MachineFunction &MF) {
  const unsigned N = Addrs.size(), NB = MF.getNumBlockIDs();
  std::vector<BitVector> Gen(NB, BitVector(N)), Kill(NB, BitVector(N)),
      PhiUse(NB, BitVector(N)), LiveIn(NB, BitVector(N)),
      LiveOut(NB, BitVector(N));
  for (MachineBasicBlock &MBB : MF) {
    BitVector &G = Gen[MBB.getNumber()], &K = Kill[MBB.getNumber()];
    for (MachineInstr &MI : MBB) {
      if (MI.isDebugInstr())
        continue;
      if (MI.isPHI()) {
        for (unsigned I = 1, E = MI.getNumOperands(); I + 1 < E; I += 2) {
          const int A = addressIndex(MI.getOperand(I));
          if (A >= 0)
            PhiUse[MI.getOperand(I + 1).getMBB()->getNumber()].set(A);
        }
      } else {
        for (const MachineOperand &MO : MI.uses()) {
          const int A = addressIndex(MO);
          if (A >= 0 && !K.test(A))
            G.set(A);
        }
      }
      for (const MachineOperand &MO : MI.defs()) {
        const int A = addressIndex(MO);
        if (A >= 0)
          K.set(A);
      }
    }
  }
  for (bool Changed = true; Changed;) {
    Changed = false;
    for (MachineBasicBlock *MBB : post_order(&MF)) {
      const unsigned B = MBB->getNumber();
      BitVector Out = PhiUse[B];
      for (MachineBasicBlock *Succ : MBB->successors())
        Out |= LiveIn[Succ->getNumber()];
      BitVector In = Out;
      In.reset(Kill[B]);
      In |= Gen[B];
      LiveOut[B] = std::move(Out);
      if (In != LiveIn[B]) {
        LiveIn[B] = std::move(In);
        Changed = true;
      }
    }
  }

  MaxLive = 0;
  MaxBlock = nullptr;
  for (MachineBasicBlock &MBB : MF) {
    auto Note = [&](const BitVector &Live, MachineBasicBlock::iterator Pos) {
      const unsigned C = Live.count();
      if (C > MaxLive) {
        MaxLive = C;
        MaxSet = Live;
        MaxBlock = &MBB;
        MaxPos = Pos;
      }
    };
    BitVector Live = LiveOut[MBB.getNumber()];
    Note(Live, MBB.end());
    for (MachineBasicBlock::iterator I = MBB.end(), B = MBB.begin(); I != B;) {
      MachineBasicBlock::iterator Next = I;
      MachineInstr &MI = *--I;
      if (MI.isDebugInstr())
        continue;
      // Right after MI: what is live there and what MI defines.
      BitVector After = Live;
      for (const MachineOperand &MO : MI.defs()) {
        const int A = addressIndex(MO);
        if (A >= 0) {
          After.set(A);
          Live.reset(A);
        }
      }
      Note(After, Next);
      if (MI.isPHI())
        continue;
      for (const MachineOperand &MO : MI.uses()) {
        const int A = addressIndex(MO);
        if (A >= 0)
          Live.set(A);
      }
      Note(Live, I);
    }
  }
}

/// Whether the address can be computed again where it is used: an
/// unpredicated gen_addr of an immediate tensor, not used by a PHI.
bool TPCAddressRemat::isRecomputable(Register R) const {
  if (Recomputed.count(R))
    return false;
  const MachineInstr *Def = MRI->getUniqueVRegDef(R);
  if (!Def ||
      (Def->getOpcode() != TPC::GEN_ADDR_ld &&
       Def->getOpcode() != TPC::GEN_ADDR_st) ||
      Def->getNumOperands() <= OpPolarity)
    return false;
  const MachineOperand &Tensor = Def->getOperand(OpTensor),
                       &Coords = Def->getOperand(OpCoords),
                       &Pred = Def->getOperand(OpPred),
                       &Polarity = Def->getOperand(OpPolarity);
  if (!Tensor.isImm() || !Coords.isReg() || !Coords.getReg().isVirtual() ||
      !Pred.isReg() || Pred.getReg() != TPC::SPRF_TRUE || !Polarity.isImm() ||
      Polarity.getImm() != 0 || !Def->isRegTiedToDefOperand(OpIncome))
    return false;
  for (const MachineInstr &Use : MRI->use_nodbg_instructions(R))
    if (Use.isPHI())
      return false;
  return true;
}

/// Instructions from the most-pressure point to the address's next use in
/// that block; ~0u when the block does not use it after that point.
unsigned TPCAddressRemat::nextUse(Register R) const {
  unsigned Dist = 0;
  for (MachineBasicBlock::iterator I = MaxPos, E = MaxBlock->end(); I != E;
       ++I) {
    if (I->isDebugInstr())
      continue;
    if (I->readsRegister(R, /*TRI=*/nullptr))
      return Dist;
    ++Dist;
  }
  return ~0u;
}

/// The gen_addrs computing the address at each use would execute: 8 per
/// level of loop around a use.
uint64_t TPCAddressRemat::cost(Register R) const {
  uint64_t C = 0;
  for (const MachineInstr &Use : MRI->use_nodbg_instructions(R))
    C += uint64_t(1) << (3 * std::min(MLI->getLoopDepth(Use.getParent()), 20u));
  return C;
}

/// Computes the address again right before each of its uses.
void TPCAddressRemat::recompute(MachineFunction &MF, Register R) {
  MachineInstr *Def = MRI->getUniqueVRegDef(R);
  LLVM_DEBUG(dbgs() << "Recompute at its uses: " << *Def);
  const Register Coords = Def->getOperand(OpCoords).getReg();
  const TargetRegisterClass *RC = MRI->getRegClass(R);
  DenseMap<MachineInstr *, Register> Done;
  for (MachineOperand &MO : llvm::make_early_inc_range(MRI->use_operands(R))) {
    MachineInstr *Use = MO.getParent();
    if (Use->isDebugInstr()) {
      MO.setReg(Register());
      continue;
    }
    Register &New = Done[Use];
    if (!New) {
      New = MRI->createVirtualRegister(RC);
      const Register Income = MRI->createVirtualRegister(RC);
      BuildMI(*Use->getParent(), Use, Def->getDebugLoc(),
              TII->get(TargetOpcode::IMPLICIT_DEF), Income);
      MachineInstr *Gen = MF.CloneMachineInstr(Def);
      Gen->getOperand(OpDest).setReg(New);
      Gen->getOperand(OpCoords).setIsKill(false);
      Gen->getOperand(OpIncome).setReg(Income);
      Gen->getOperand(OpIncome).setIsKill(false);
      Use->getParent()->insert(Use->getIterator(), Gen);
      Recomputed.insert(New);
      ++NumGenAddrs;
    }
    MO.setReg(New);
  }
  MRI->clearKillFlags(Coords);
  Def->eraseFromParent();
  ++NumAddresses;
}

bool TPCAddressRemat::runOnMachineFunction(MachineFunction &MF) {
  MRI = &MF.getRegInfo();
  if (!MRI->isSSA())
    return false;
  unsigned Limit = LiveAddresses;
  if (!Limit) {
    const BitVector Reserved =
        MF.getSubtarget().getRegisterInfo()->getReservedRegs(MF);
    for (MCPhysReg Reg : TPC::ADRFRegClass)
      if (!Reserved.test(Reg))
        ++Limit;
  }
  TII = MF.getSubtarget().getInstrInfo();
  MLI = &getAnalysis<MachineLoopInfoWrapperPass>().getLI();
  bool Changed = false;
  while (collect(MF) && Addrs.size() > Limit) {
    measure(MF);
    if (MaxLive <= Limit)
      break;
    LLVM_DEBUG({
      dbgs() << MF.getName() << ": " << MaxLive << " addresses live in "
             << printMBBReference(*MaxBlock) << " before ";
      if (MaxPos == MaxBlock->end())
        dbgs() << "its end\n";
      else
        dbgs() << *MaxPos;
      for (unsigned A : MaxSet.set_bits())
        dbgs() << ' ' << printReg(Addrs[A]);
      dbgs() << '\n';
    });
    Register Best;
    unsigned BestDist = 0;
    uint64_t BestCost = 0;
    for (unsigned A : MaxSet.set_bits()) {
      const Register R = Addrs[A];
      if (!isRecomputable(R))
        continue;
      const unsigned Dist = nextUse(R);
      const uint64_t C = cost(R);
      if (!Best || Dist > BestDist || (Dist == BestDist && C < BestCost)) {
        Best = R;
        BestDist = Dist;
        BestCost = C;
      }
    }
    if (!Best)
      break;
    recompute(MF, Best);
    Changed = true;
  }
  Recomputed.clear();
  return Changed;
}
