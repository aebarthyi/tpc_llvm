// Unsigned vector remainder. Operands of 2^31 or more follow Habana's compiler, not
// C (see README.md).
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  u32 256 6 1 range=0,4294967295
// DIFFTEST: in  u32 256 6 1 range=0,4294967295
// DIFFTEST: out u32 256 6 1
// DIFFTEST: index-space 4 6 1

void main(tensor a, tensor b, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int y = start[1]; y < end[1]; y++) {
    i[1] = y;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      uint64 n = v_u32_ld_tnsr_b(i, a);
      // Shift some divisors down, so the remainders are not nearly all equal to n.
      uint64 d = (y & 1 ? v_u32_ld_tnsr_b(i, b) >> 12 : v_u32_ld_tnsr_b(i, b)) | 1;
      v_u32_st_tnsr(i, out, n % d);
    }
  }
}
