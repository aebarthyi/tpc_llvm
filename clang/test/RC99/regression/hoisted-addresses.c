// RUN: %tpc_clang -march=gaudi2 -O2 -S %s -o - 2>&1 | FileCheck %s
// RUN: %tpc_clang -march=gaudi2 -O3 -S %s -o - 2>&1 | FileCheck %s
// RUN: %tpc_clang -march=gaudi3 -O2 -S %s -o - 2>&1 | FileCheck %s

// The slot loop is unrolled fully, and LICM moves its 12 gen_addrs, which do
// not depend on the block, out of the block loop: 12 addresses live across
// it, of the 8 address registers on gaudi2. An address register cannot be
// spilled (it also holds its tensor), and this failed with "too many
// addresses are live at once". 5 of them are now computed again in the loop,
// right before their loads (TPCAddressRemat). gaudi3 has 16.

void main(tensor ids, tensor scale, tensor channel, tensor out) {
  const int5 start = get_index_space_offset(), end = start + get_index_space_size();
  for (int token = start[1]; token < end[1]; ++token) {
    for (int block = start[0]; block < end[0]; ++block) {
      float64 acc = 0;
      for (int slot = 0; slot < 6; ++slot) {
        const int row = token * 6 + slot;
        const int e = s_i32_ld_g(gen_addr((int5){row}, ids));
        const float sx = s_f32_ld_g(gen_addr((int5){0, row}, scale));
        acc += v_f32_ld_tnsr_b((int5){block * 64, e}, channel) * sx;
      }
      v_f32_st_tnsr((int5){block * 64, token}, out, acc);
    }
  }
}

// CHECK-NOT: error
// CHECK-NOT: warning
// CHECK-NOT: %Z30
// CHECK:     ld_g
// CHECK:     halt
