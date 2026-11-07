//===--- TPC.cpp - TPC ABI and target code generation info ----------------===//
//
//===----------------------------------------------------------------------===//
//
// This file implements TPC's ABIInfo and TargetCodeGenInfo.
//
//===----------------------------------------------------------------------===//

#ifdef LLVM_TPC_COMPILER
#include "ABIInfoImpl.h"
#include "TargetInfo.h"

using namespace clang;
using namespace clang::CodeGen;

namespace {
class TPCABIInfo : public DefaultABIInfo {
public:
  TPCABIInfo(CodeGen::CodeGenTypes &CGT) : DefaultABIInfo(CGT) {}
};

class TPCTargetCodeGenInfo : public TargetCodeGenInfo {
public:
  TPCTargetCodeGenInfo(CodeGen::CodeGenTypes &CGT)
      : TargetCodeGenInfo(std::make_unique<TPCABIInfo>(CGT)) {}

  /// Get target favored AST address space of a global variable for languages
  /// other than OpenCL and CUDA.
  /// If \p D is nullptr, returns the default target favored address space
  /// for global variable.
  LangAS getGlobalVarAddressSpace(CodeGenModule &CGM,
                                  const VarDecl *D) const override {
    LangAS AddrSpace = D->getType().getAddressSpace();
    switch (AddrSpace) {
    case LangAS::rc99_local_scalar:
    case LangAS::rc99_local_vector:
    case LangAS::rc99_global:
      return AddrSpace;
    case LangAS::Default:
      // If address space is not set, consider reference to local address space.
    case LangAS::opencl_local:
      if (isInTPCVectorAddressSpace(D->getType()))
        return LangAS::rc99_local_vector;
      return LangAS::rc99_local_scalar;
    case LangAS::opencl_global:
      return LangAS::rc99_global;
    default:
      llvm_unreachable("Unexpected address space");
    }
  }
};
} // namespace

std::unique_ptr<TargetCodeGenInfo>
CodeGen::createTPCTargetCodeGenInfo(CodeGenModule &CGM) {
  return std::make_unique<TPCTargetCodeGenInfo>(CGM.getTypes());
}
#endif // LLVM_TPC_COMPILER
