// RUN: %tpc_clang -O0 -c -x c -march=gaudi2 %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s

// .tpc_metadata records direct MMIO access at every -O level, including -O0, where
// functions are optnone.

void main(int x) {
  int v = s_i32_ld_l(0x100, SW_MMIO, 0, 1, 0);
  s_i32_st_l(0x100, v + x, SW_MMIO, 1, 0);
}

// CHECK: directMMIOAccess: DB 1
