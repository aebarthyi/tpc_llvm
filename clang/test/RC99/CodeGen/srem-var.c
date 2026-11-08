// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi %s -o - | FileCheck %s --check-prefixes=CHECK,STEP
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu goya2 %s -o - | FileCheck %s --check-prefixes=CHECK,STEP
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi2 %s -o - | FileCheck %s --check-prefixes=CHECK,GEN4

void main(int a, int b, int dest) {
  *(int __local *)dest = a % b;
}

// CHECK-DAG: abs.i32 [[ABS_B:%S[0-9]+]], %S1
// CHECK-DAG: abs.i32 [[ABS_A:%S[0-9]+]], %S0
// CHECK-DAG: cmp_less.i32 [[NEG:%SP[0-9]+]], %S0, 0x0
// STEP:      udiv_4step.u32 {{.*}}, [[ABS_B]]
// GEN4:      udiv.u32 mod {{%S[0-9]+}}, [[ABS_A]], [[ABS_B]]
// CHECK:     sub.i32 neg [[RESULT:%S[0-9]+]], {{%S[0-9]+}}, 0x0, [[NEG]]
// CHECK:     st_l %S2, [[RESULT]]
