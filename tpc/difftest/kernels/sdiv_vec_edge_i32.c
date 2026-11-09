// Signed vector `/` on the inputs where the hardware and C disagree or C has no
// answer: INT_MIN, whose absolute value saturates, and a zero divisor. The
// reference for these is Habana's compiler, not C.
//
// Numerators are 0..3 and INT_MIN..INT_MIN+3. Odd rows divide by -4..3, which
// includes 0 and -1. Even rows divide by those same numerator values, which
// includes 0 and INT_MIN.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  i32 256 6 1 range=-2147483648,2147483647
// DIFFTEST: in  i32 256 6 1 range=-2147483648,2147483647
// DIFFTEST: out i32 256 6 1
// DIFFTEST: index-space 4 6 1

void main(tensor a, tensor b, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};
  const int edge = 0x80000003;

  for (int y = start[1]; y < end[1]; y++) {
    i[1] = y;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      int64 n = v_i32_ld_tnsr_b(i, a) & edge;
      int64 d = v_i32_ld_tnsr_b(i, b);
      if (y & 1)
        d = (d & 7) - 4;
      else
        d = d & edge;
      v_i32_st_tnsr(i, out, n / d);
    }
  }
}
