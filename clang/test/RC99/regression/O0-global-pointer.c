// RUN: %tpc_clang -S -O0 -march=gaudi2 %s -o - | FileCheck %s

// A global pointer has to stay in a register at -O0, since nothing can load or
// store it. InstCombine would forward it within a block, so use it in another one.

void main(unsigned dest, tensor out, _Bool cond) {
  int5 indx = {0, 0, 0, 0, 0};
  int __global *addr = gen_addr(indx, out, 0, 0, 1, 0);
  int __local *dptr = (int __local *)dest;
  if (cond)
    *dptr = s_i32_ld_g(addr, 0, 0, 0, 1);
}

// CHECK: ld_g %S{{[0-9]+}}, %AD{{[0-9]+}}
