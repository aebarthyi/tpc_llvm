// RUN: %tpc_clang -march=gaudi2 -O2 -S -mllvm -tpc-modulo-schedule=false %s -o - | FileCheck %s --check-prefix=ROOM
// RUN: %tpc_clang -march=gaudi2 -O2 -S -mllvm -tpc-modulo-schedule=false -DFULL %s -o - | FileCheck %s --check-prefix=FULL
// RUN: %tpc_clang -march=gaudi2 -O2 -S -mllvm -tpc-modulo-schedule=false -DFULL -mllvm -tpc-chain-unroll-spill-room=0 %s -o - | FileCheck %s --check-prefix=ROOM

// The IR unroller unrolls a loop that carries values (here 8 MAC
// accumulators) up to 4 times, in order. Spills go to the vector local
// memory the kernel's __local__ data leaves free; when less than 16 KB is
// left (-DFULL: 72 KB of the 80 KB), the loop is unrolled only as far as its
// copies' vector values fit the 40 vector registers: the value live through
// the loop (g), the 8 accumulators once, and per copy the other values live
// at one point (the 8 products and a load). 4 copies need 1 + 8 + 4 * 9 = 45,
// 2 copies 27. With room to spare (or -tpc-chain-unroll-spill-room=0) the
// spills cost less than the overlap of the copies gains, and the loop is
// unrolled 4 times. wright's gdn_verify, whose 64 KB recurrent state stays
// in VLM, failed to build with "Too many vector spills" before.

#ifdef FULL
#define ROWS 288
#else
#define ROWS 32
#endif

__local__ float64 S[ROWS];

void main(tensor in, tensor out, int n) {
  int5 c = 0;
  const float64 g = v_f32_ld_tnsr_b(c, in);
  float64 a0 = 0, a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0, a7 = 0;
  for (int t = 0; t < n; t++) {
    __local__ float64 *s = &S[(t & 3) * 8];
    const float64 r0 = s[0] * g, r1 = s[1] * g, r2 = s[2] * g, r3 = s[3] * g;
    const float64 r4 = s[4] * g, r5 = s[5] * g, r6 = s[6] * g, r7 = s[7] * g;
    s[0] = r0; s[1] = r1; s[2] = r2; s[3] = r3;
    s[4] = r4; s[5] = r5; s[6] = r6; s[7] = r7;
    a0 = v_f32_mac_b(r0, r0, a0); a1 = v_f32_mac_b(r1, r1, a1);
    a2 = v_f32_mac_b(r2, r2, a2); a3 = v_f32_mac_b(r3, r3, a3);
    a4 = v_f32_mac_b(r4, r4, a4); a5 = v_f32_mac_b(r5, r5, a5);
    a6 = v_f32_mac_b(r6, r6, a6); a7 = v_f32_mac_b(r7, r7, a7);
  }
  v_f32_st_tnsr(c, out, ((a0 + a1) + (a2 + a3)) + ((a4 + a5) + (a6 + a7)));
}

// ROOM:     loop 0, %S{{[0-9]+}}, 4, <,
// ROOM-NOT: loop 0, %S{{[0-9]+}}, 2, <,

// FULL:     loop 0, %S{{[0-9]+}}, 2, <,
// FULL-NOT: loop 0, %S{{[0-9]+}}, 4, <,
