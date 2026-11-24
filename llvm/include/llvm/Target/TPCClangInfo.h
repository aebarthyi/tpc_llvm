#ifndef TPC_CLANG_INFO_H
#define TPC_CLANG_INFO_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

#include <cstdint>
#include <string>
namespace llvm {
extern std::string TPCCompilerArguments;
extern bool UsePrintf;
extern unsigned NumArgumentTensors;

std::string makeTPCCompilerArgumentsString(ArrayRef<const char *> Args);

/// Module flag in which clang passes the kernel's hashedISA value to the
/// backend, so that -S and -c agree although only -c embeds the source text.
constexpr const char TPCKernelHashFlag[] = "tpc-kernel-hash";

/// The hashedISA value of a kernel: CRC-32 of the source file's stem, then of
/// the source text.
uint32_t computeTPCKernelHash(StringRef SourceFileName, StringRef SourceText);

/// Function attributes of the code generation retry for vector spills. On
/// the first run clang gives the kernel TPCSpillRetryAttr: if its spills do
/// not fit in the VLM its data leaves, the backend marks it
/// TPCSpillOverflowAttr instead of failing with "Too many vector spills", and
/// clang generates code again from a copy of the module whose kernel has
/// TPCShortOfVlmAttr, under which the machine scheduler keeps every block
/// within the vector registers.
constexpr const char TPCSpillRetryAttr[] = "tpc-spill-retry";
constexpr const char TPCSpillOverflowAttr[] = "tpc-spill-overflow";
constexpr const char TPCShortOfVlmAttr[] = "tpc-short-of-vlm";
}

#endif // TPC_CLANG_INFO_H
