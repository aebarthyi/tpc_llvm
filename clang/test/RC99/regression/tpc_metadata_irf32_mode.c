// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=IRF32
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -long-irf %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=IRF44

void main(tensor in, tensor out) {
  float64 v = v_f32_ld_tnsr_b(get_index_space_offset(), in);
  v_f32_st_tnsr(get_index_space_offset(), out, v);
}

// IRF32: irf32Mode: DB 1
// IRF44: irf32Mode: DB 0
