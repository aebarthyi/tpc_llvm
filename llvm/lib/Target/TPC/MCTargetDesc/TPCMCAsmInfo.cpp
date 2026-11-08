//===---- TPCMCAsmInfo.cpp ------------------------------------------------===//
//
//===----------------------------------------------------------------------===//
//
// This file contains the definition of the TPCMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#include "TPCMCAsmInfo.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

void TPCMCAsmInfo::anchor() { }

TPCMCAsmInfo::TPCMCAsmInfo(const Triple &TheTriple,
                           const MCTargetOptions &Options)
    : MCAsmInfoELF(Options) {
  assert(TheTriple.getArch() == Triple::tpc);

  IsLittleEndian = true;
  ExceptionsType = ExceptionHandling::None;

  CommentString = "//";
  SupportsDebugInformation = true;
  InternalSymbolPrefix = "$";
  BlockLabelPrefix = ".L";
}
