// RUN: %tpc_clang -march=gaudi2 -O2 -S %s -o - | FileCheck %s
// RUN: %tpc_clang -march=gaudi2 -O0 -S %s -o - | FileCheck %s

// A vector compare used as a value is -1 where it holds and 0 elsewhere:
// `sext <64 x i1> to <64 x i32>`, which TPC has no instruction for. Both the
// fork and Habana's compiler stopped with "Cannot select: sign_extend". It is
// now selected like the zero extend: a move of -1 where the compare holds and
// of 0 where it does not.

void main(tensor ti, tensor tf, tensor ts, tensor tc, tensor out) {
  const int64 a = v_i32_ld_tnsr_b((int5){0, 0}, ti), b = v_i32_ld_tnsr_b((int5){0, 1}, ti);
  const float64 fa = v_f32_ld_tnsr_b((int5){0, 0}, tf), fb = v_f32_ld_tnsr_b((int5){0, 1}, tf);
  const short128 sa = v_i16_ld_tnsr_b((int5){0, 0}, ts), sb = v_i16_ld_tnsr_b((int5){0, 1}, ts);
  const char256 ca = v_i8_ld_tnsr_b((int5){0, 0}, tc), cb = v_i8_ld_tnsr_b((int5){0, 1}, tc);
  v_i32_st_tnsr((int5){0, 0}, out, a <= b);
  v_i32_st_tnsr((int5){0, 1}, out, fa < fb);
  v_i16_st_tnsr((int5){0, 2}, out, sa > sb);
  v_i8_st_tnsr((int5){0, 3}, out, ca == cb);
}

// CHECK-DAG: cmp_leq.i32 [[P32:%VP[0-9]+]], %V{{[0-9]+}}, %V{{[0-9]+}}
// CHECK-DAG: mov.i32 [[M32:%V[0-9]+]], -0x1, [[P32]]
// CHECK-DAG: mov.i32 [[M32]], 0x0, ![[P32]]
// CHECK-DAG: cmp_less.f32 [[PF:%VP[0-9]+]], %V{{[0-9]+}}, %V{{[0-9]+}}
// CHECK-DAG: mov.i32 [[MF:%V[0-9]+]], -0x1, [[PF]]
// CHECK-DAG: mov.i32 [[MF]], 0x0, ![[PF]]
// CHECK-DAG: cmp_grt.i16 [[P16:%VP[0-9]+]], %V{{[0-9]+}}, %V{{[0-9]+}}
// CHECK-DAG: mov.i16 [[M16:%V[0-9]+]], -0x1, [[P16]]
// CHECK-DAG: mov.i16 [[M16]], 0x0, ![[P16]]
// CHECK-DAG: cmp_eq.i8 [[P8:%VP[0-9]+]], %V{{[0-9]+}}, %V{{[0-9]+}}
// CHECK-DAG: mov.i8 [[M8:%V[0-9]+]], -0x1, [[P8]]
// CHECK-DAG: mov.i8 [[M8]], 0x0, ![[P8]]
