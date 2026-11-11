// Signed vector remainder.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  i32 256 6 1 range=-2000000000,2000000000
// DIFFTEST: in  i32 256 6 1 range=-100000,100000
// DIFFTEST: out i32 256 6 1
// DIFFTEST: index-space 4 6 1

void main(tensor a, tensor b, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int y = start[1]; y < end[1]; y++) {
    i[1] = y;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      int64 n = v_i32_ld_tnsr_b(i, a);
      int64 d = v_i32_ld_tnsr_b(i, b) | 1; // never 0
      v_i32_st_tnsr(i, out, n % d);
    }
  }
}
