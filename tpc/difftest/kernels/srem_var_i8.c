// Signed remainder of chars. At -O2 the backend sees a narrowed srem i16.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  i32 256 6 2 range=-1000,1000
// DIFFTEST: out i32 256 6 2
// DIFFTEST: index-space 4 6 2
// DIFFTEST: scalar i32 -123
// DIFFTEST: scalar i32 101

void main(tensor a, tensor out, int n0, int p0) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};
  char n = n0;
  char p = p0;

  for (int y = start[1]; y < end[1]; y++) {
    i[1] = y;
    char d = y * 5 - 12; // -12 -7 -2 3 8 13
    char rn = n % d;
    char rp = p % d;
    int wn = rn;
    int wp = rp;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      i[2] = 0;
      v_i32_st_tnsr(i, out, v_i32_ld_tnsr_b(i, a) + wn);
      i[2] = 1;
      v_i32_st_tnsr(i, out, v_i32_ld_tnsr_b(i, a) + wp);
    }
  }
}
