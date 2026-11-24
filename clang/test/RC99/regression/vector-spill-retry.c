// RUN: %tpc_clang -march=gaudi2 -O2 -S %s -o - | FileCheck %s
// RUN: not %tpc_clang -march=gaudi2 -O2 -S -DKEEP=40 -DROWS=320 %s -o /dev/null 2>&1 | FileCheck %s --check-prefix=ERR

// S takes all but 4 vectors of the 80 KB VLM, so few vector spills fit. The
// machine scheduler lets each block exceed the 40 vector registers by the
// spills the VLM can hold, counting only the registers the block uses: the
// loop below uses 17 when its loads are scheduled early, but 36 vectors
// (keep) live through it unused, and the register allocator spilled 16
// vectors, which failed with "Too many vector spills". The backend now reports
// that overflow to clang, which generates the code again from a copy of the
// module with every block scheduled within the registers, the live-through
// vectors counted, so the loop keeps its original order and nothing spills.
//
// When even that does not fit (-DKEEP=40 -DROWS=320: 40 vectors live
// through and no VLM left), the second attempt reports the error, once.

#ifndef KEEP
#define KEEP 36
#endif
#ifndef ROWS
#define ROWS 316
#endif
__local__ float64 S[ROWS];

void main(tensor in, tensor out, int n) {
  int5 c = 0;
  float64 keep[KEEP];
#pragma unroll
  for (int i = 0; i < KEEP; i++) {
    c[1] = i;
    keep[i] = v_f32_ld_tnsr_b(c, in);
  }
  float64 acc = 0;
  for (int t = 0; t < n; t++) {
    c[2] = t;
#pragma unroll
    for (int i = 0; i < 12; i++) {
      c[1] = i;
      acc = v_f32_mac_b(v_f32_ld_tnsr_b(c, in), S[i + (t & 3)], acc);
    }
    S[t & 3] = acc;
  }
  float64 r = acc;
#pragma unroll
  for (int i = 0; i < KEEP; i++)
    r += keep[i];
  c[1] = 0;
  c[2] = 0;
  v_f32_st_tnsr(c, out, r);
}

// CHECK: main:
// CHECK: halt

// ERR:     error in backend: Too many vector spills
// ERR-NOT: Too many vector spills
