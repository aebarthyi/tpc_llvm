//===- sim_runner.cpp - Run one TPC kernel on the Habana TPC simulator ----===//
//
// Built and driven by run.py; not part of the LLVM build. It links against the
// habanatools simulator libraries from $TPC_HOME, which are not redistributed
// with this repository.
//
// Usage: sim_runner <spec-file>
//
// The spec file is written by run.py, one directive per line:
//   device <gaudi|gaudi2|gaudi3>
//   elf <path to kernel object produced by tpc-clang -c>
//   index_space <n0> <n1> <n2> <n3> <n4>
//   scalar <u32 bit pattern, hex>              (repeatable, in order)
//   tensor <in|out> <elem bytes> <gc data type> <s0> <s1> <s2> <s3> <s4> <file>
//
// Inputs are read from <file>; outputs are pre-filled with 0xCD bytes so that
// unwritten elements are visible, and written to <file> after the run.
// Prints "cycles <n>" on success.
//
//===----------------------------------------------------------------------===//

#include "tpc_test_core_api.h"

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace {

using tpc_tests::TensorDesc2;

constexpr unsigned NumDims = 5;

struct TensorSpec {
  bool IsOutput = false;
  unsigned ElemSize = 0;
  uint32_t GcType = 0;
  uint64_t Sizes[NumDims] = {};
  std::string File;
  std::vector<uint8_t> Data;

  uint64_t numElements() const {
    uint64_t N = 1;
    for (uint64_t S : Sizes)
      N *= S;
    return N;
  }
};

[[noreturn]] void die(const char *Fmt, ...) {
  va_list Args;
  va_start(Args, Fmt);
  std::fprintf(stderr, "sim_runner: error: ");
  std::vfprintf(stderr, Fmt, Args);
  std::fprintf(stderr, "\n");
  va_end(Args);
  std::exit(2);
}

std::vector<uint8_t> readFile(const std::string &Path) {
  std::ifstream In(Path, std::ios::binary);
  if (!In)
    die("cannot read '%s'", Path.c_str());
  return {std::istreambuf_iterator<char>(In), std::istreambuf_iterator<char>()};
}

void writeFile(const std::string &Path, const std::vector<uint8_t> &Data) {
  std::ofstream Out(Path, std::ios::binary);
  Out.write(reinterpret_cast<const char *>(Data.data()), Data.size());
  if (!Out)
    die("cannot write '%s'", Path.c_str());
}

tpc_lib_api::DeviceId parseDevice(const std::string &Name) {
  if (Name == "gaudi")
    return tpc_lib_api::DEVICE_ID_GAUDI;
  if (Name == "gaudi2")
    return tpc_lib_api::DEVICE_ID_GAUDI2;
  if (Name == "gaudi3")
    return tpc_lib_api::DEVICE_ID_GAUDI3;
  die("device '%s' is not supported by the simulator", Name.c_str());
}

// Element-size encoding used by the TPC tensor descriptor.
uint32_t elemSizeType(unsigned Bytes) {
  switch (Bytes) {
  case 1: return 0;
  case 2: return 1;
  case 4: return 2;
  case 8: return 11;
  }
  die("unsupported element size %u", Bytes);
}

TensorDesc2 makeDescriptor(TensorSpec &T) {
  TensorDesc2 D = {};
  D.baseAddrUnion.baseAddr = reinterpret_cast<uint64_t>(T.Data.data());
  uint32_t Stride = 1; // In elements.
  for (unsigned I = 0; I < NumDims; ++I) {
    D.dimDescriptors[I].size = static_cast<uint32_t>(T.Sizes[I]);
    D.dimDescriptors[I].stride = Stride;
    Stride *= static_cast<uint32_t>(T.Sizes[I]);
  }
  const uint32_t ValidDimMask = (1u << NumDims) - 1;
  const uint32_t LastDim = NumDims - 1;
  D.configuration = (elemSizeType(T.ElemSize) & 0x3) | (ValidDimMask << 8) |
                    ((LastDim & 0x7) << 16);
  return D;
}

void fillGeometry(tpc_lib_api::Tensor &Out, const TensorSpec &T) {
  Out.geometry.dims = NumDims;
  Out.geometry.dataType = static_cast<tpc_lib_api::TensorDataType>(T.GcType);
  for (unsigned I = 0; I < NumDims; ++I)
    Out.geometry.maxSizes[I] = Out.geometry.minSizes[I] = T.Sizes[I];
}

} // namespace

int main(int Argc, char **Argv) {
  if (Argc != 2)
    die("usage: sim_runner <spec-file>");

  std::ifstream Spec(Argv[1]);
  if (!Spec)
    die("cannot read spec '%s'", Argv[1]);

  std::string DeviceName, ElfPath;
  uint64_t IndexSpace[NumDims] = {1, 1, 1, 1, 1};
  std::vector<uint32_t> Scalars;
  std::vector<TensorSpec> Inputs, Outputs;

  std::string Line;
  while (std::getline(Spec, Line)) {
    std::istringstream LS(Line);
    std::string Key;
    if (!(LS >> Key))
      continue;
    if (Key == "device") {
      LS >> DeviceName;
    } else if (Key == "elf") {
      LS >> ElfPath;
    } else if (Key == "index_space") {
      for (uint64_t &N : IndexSpace)
        LS >> N;
    } else if (Key == "scalar") {
      std::string Hex;
      LS >> Hex;
      Scalars.push_back(static_cast<uint32_t>(std::stoul(Hex, nullptr, 16)));
    } else if (Key == "tensor") {
      TensorSpec T;
      std::string Dir;
      LS >> Dir >> T.ElemSize >> T.GcType;
      for (uint64_t &S : T.Sizes)
        LS >> S;
      LS >> T.File;
      T.IsOutput = Dir == "out";
      (T.IsOutput ? Outputs : Inputs).push_back(std::move(T));
    } else {
      die("unknown spec directive '%s'", Key.c_str());
    }
    if (LS.fail())
      die("malformed spec line: %s", Line.c_str());
  }
  if (ElfPath.empty() || DeviceName.empty())
    die("spec must set 'device' and 'elf'");
  if (Scalars.size() > tpc_lib_api::MAX_SCALAR_PARAMS)
    die("too many scalar params (%zu)", Scalars.size());

  for (TensorSpec &T : Inputs) {
    T.Data = readFile(T.File);
    if (T.Data.size() != T.numElements() * T.ElemSize)
      die("'%s' has %zu bytes, expected %llu", T.File.c_str(), T.Data.size(),
          static_cast<unsigned long long>(T.numElements() * T.ElemSize));
  }
  for (TensorSpec &T : Outputs)
    T.Data.assign(T.numElements() * T.ElemSize, 0xCD);

  std::vector<uint8_t> Elf = readFile(ElfPath);

  // Kernel parameters, as the graph compiler would pass them to glue code.
  std::vector<tpc_lib_api::Tensor> InTensors(Inputs.size()),
      OutTensors(Outputs.size());
  for (size_t I = 0; I < Inputs.size(); ++I)
    fillGeometry(InTensors[I], Inputs[I]);
  for (size_t I = 0; I < Outputs.size(); ++I)
    fillGeometry(OutTensors[I], Outputs[I]);

  tpc_lib_api::HabanaKernelParams Params;
  std::memset(&Params, 0, sizeof(Params));
  Params.deviceId = parseDevice(DeviceName);
  std::strncpy(Params.guid.name, "difftest", tpc_lib_api::MAX_NODE_NAME - 1);
  Params.inputTensors = InTensors.data();
  Params.inputTensorNr = static_cast<uint32_t>(InTensors.size());
  Params.outputTensors = OutTensors.data();
  Params.outputTensorNr = static_cast<uint32_t>(OutTensors.size());

  // Kernel instantiation, as glue code would return it. Every tensor is
  // declared allRequired: we compare outputs, not access-pattern coverage.
  std::vector<tpc_lib_api::TensorAccessPattern> InAP(Inputs.size() + 1),
      OutAP(Outputs.size() + 1);
  for (auto &AP : InAP)
    AP.allRequired = 1;
  for (auto &AP : OutAP)
    AP.allRequired = 1;
  std::vector<tpc_lib_api::AuxTensor> Aux(1);

  tpc_lib_api::HabanaKernelInstantiation Inst;
  std::memset(&Inst, 0, sizeof(Inst));
  Inst.indexSpaceRank = NumDims;
  for (unsigned I = 0; I < NumDims; ++I)
    Inst.indexSpaceGeometry[I] = IndexSpace[I];
  Inst.inputTensorAccessPattern = InAP.data();
  Inst.outputTensorAccessPattern = OutAP.data();
  Inst.auxiliaryTensors = Aux.data();
  Inst.auxiliaryTensorNr = 0;
  Inst.kernel.kernelElf = Elf.data();
  Inst.kernel.elfSize = static_cast<uint32_t>(Elf.size());
  Inst.kernel.paramsNr = static_cast<uint32_t>(Scalars.size());
  for (size_t I = 0; I < Scalars.size(); ++I)
    Inst.kernel.scalarParams[I] = Scalars[I];

  std::vector<TensorDesc2> Descs;
  for (TensorSpec &T : Inputs)
    Descs.push_back(makeDescriptor(T));
  for (TensorSpec &T : Outputs)
    Descs.push_back(makeDescriptor(T));

  VPEStats Stats;
  unsigned Cycles = tpc_tests::RunSimulation(
      Params, Inst, Descs, Stats, tpc_tests::e_accessPatternIgnoreMode);

  for (TensorSpec &T : Outputs)
    writeFile(T.File, T.Data);
  std::printf("cycles %u\n", Cycles);
  return 0;
}
