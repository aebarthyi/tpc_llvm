// Scalar pipeline: integer division/modulo of kernel parameters and loop
// indices (TPC has no divide instruction, so this is lowered in software),
// broadcast into vector arithmetic. Signed remainder by a variable divisor
// is covered separately by srem_var_i32.c.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  i32 256 6 3 range=-100000,100000
// DIFFTEST: out i32 256 6 3
// DIFFTEST: index-space 4 6 3
// DIFFTEST: scalar i32 1234567
// DIFFTEST: scalar i32 -37

void main(tensor a, tensor out, int n, int d) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      int k = (n / (d - y)) % 7 + (int)((unsigned)n % (unsigned)(y + z + 3));
      unsigned u = (unsigned)n / (unsigned)(y + 5);
      for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
        i[0] = x;
        int64 v = v_i32_ld_tnsr_b(i, a);
        v_i32_st_tnsr(i, out, v * k + (int)u + x / 64);
      }
    }
  }
}
