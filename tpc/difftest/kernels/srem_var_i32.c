// Signed scalar remainder by a non-constant divisor, for every sign combination.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  i32 256 6 2
// DIFFTEST: out i32 256 6 2
// DIFFTEST: index-space 4 6 2
// DIFFTEST: scalar i32 -1234567
// DIFFTEST: scalar i32 987654

void main(tensor a, tensor out, int n, int p) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int y = start[1]; y < end[1]; y++) {
    i[1] = y;
    int d = y * 5 - 12; // -12 -7 -2 3 8 13
    int rn = n % d;
    int rp = p % d;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      i[2] = 0;
      v_i32_st_tnsr(i, out, v_i32_ld_tnsr_b(i, a) + rn);
      i[2] = 1;
      v_i32_st_tnsr(i, out, v_i32_ld_tnsr_b(i, a) + rp);
    }
  }
}
