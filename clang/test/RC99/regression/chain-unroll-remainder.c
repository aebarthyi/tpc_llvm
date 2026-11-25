// RUN: %tpc_clang -march=gaudi2 -O2 -S %s -o - | FileCheck %s
// RUN: %tpc_clang -march=gaudi2 -O2 -S -mllvm -tpc-remainder-loop-trips=0 %s -o - | FileCheck --check-prefix=OFF %s

// The second loop adds what the first, 2 vectors at a time, left: it starts
// with n - i < 128 whether the first loop ran or not, so it runs at most
// twice. TPCRemainderLoops keeps it from the runtime unroller
// (TPCTTIImpl::getUnrollingPreferences unrolls a loop carrying a sum 4 times
// at run time), whose trip count and remainder computation cost more than
// the copies gain in a loop that short (wright's qsa_select entered two
// such loops 32 times per query). The first loop is still unrolled, and its
// own remainder loop follows it.

void main(tensor a, tensor out) {
  const int n = get_dim_size(a, 0);
  int5 c = 0;
  float64 s0 = 0.0f, s1 = 0.0f;
  int i = 0;
  for (; i + 128 <= n; i += 128) {
    c[0] = i;
    s0 += v_f32_ld_tnsr_b(c, a);
    c[0] = i + 64;
    s1 += v_f32_ld_tnsr_b(c, a);
  }
  for (; i < n; i += 64) {
    c[0] = i;
    s0 += v_f32_ld_tnsr_b(c, a);
  }
  c[0] = 0;
  v_f32_st_tnsr(c, out, s0 + s1);
}

// CHECK:         loop {{.*}}, 0, -4, >,
// CHECK:         loop {{.*}}, 0, -1, >,
// CHECK:         loop {{.*}}, 64, <, [[END:\.LBB0_[0-9]+]]
// CHECK:         add.f32
// CHECK-NOT:     add.f32
// CHECK:         [[END]]:
// CHECK-NOT:     loop
// CHECK:         halt

// OFF:           loop {{.*}}, 0, -4, >,
// OFF:           loop {{.*}}, 0, -1, >,
// OFF:           loop {{.*}}, 0, -64, >,
// OFF:           halt
