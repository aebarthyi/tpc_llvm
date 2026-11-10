// RUN: %tpc_clang -S -O0 -march=gaudi2 %s -o - | FileCheck %s

// At -O0 the driver passes -tpc-debug-nops=8, which puts 8 NOPs before every
// instruction. On gaudi2 a store reads the result of UDIV only 12 cycles after
// it issues, so the latency resolver must still add the NOPs the debug ones do
// not cover. With 8 NOPs st_l stored the old value of the register.

void main(unsigned n, unsigned d) {
  s_u32_st_l(0x100, n / d);
}

// CHECK:      udiv.u32 div [[Q:%S[0-9]+]], %S0, %S1
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: st_l 0x100, [[Q]]

// The resolver also adds its own 3 NOPs after HALT to the 3 of the debug NOPs.
// CHECK:      halt
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: nop
// CHECK-NEXT: $func_end0:
