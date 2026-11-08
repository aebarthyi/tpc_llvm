//===-- TPCELFObjectWriter.cpp - Mips ELF Writer -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <list>
#include "MCTargetDesc/TPCMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"

#define DEBUG_TYPE "tpc-elf-object-writer"

using namespace llvm;

class TPCELFObjectWriter : public MCELFObjectTargetWriter {
public:
  TPCELFObjectWriter()
      : MCELFObjectTargetWriter(/*Is64Bit*/ false, 0, ELF::EM_386, // i386 for now
                                /*HasRelocationAddend*/ false) {}

  // MCELFObjectTargetWriter interface
public:
  unsigned getRelocType(const MCFixup &Fixup, const MCValue &Target,
                        bool IsPCRel) const override {
    if (Target.getSpecifier())
      llvm_unreachable("Modifier not supported");
    if (IsPCRel)
      return 0;
    switch (Fixup.getKind()) {
    case FK_Data_4:
      return ELF::R_386_32;
    default:
      llvm_unreachable("Unknown ELF relocation type");
    }
  }
};

std::unique_ptr<MCObjectTargetWriter> llvm::createTPCELFObjectWriter() {
  return std::make_unique<TPCELFObjectWriter>();
}
