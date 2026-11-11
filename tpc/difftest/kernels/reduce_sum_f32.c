// Sum over dim 1 with a loop-carried vector accumulator. Dim 1 is not split
// across the index space, so each member reduces a whole column.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  f32 256 16 2
// DIFFTEST: out f32 256 1 2
// DIFFTEST: index-space 4 1 2

void main(tensor a, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  const int rows = get_dim_size(a, 1);
  int5 i = {0, 0, 0, 0, 0};
  int5 o = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = o[2] = z;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = o[0] = x;
      float64 sum = 0.0f;
      for (int y = 0; y < rows; y++) {
        i[1] = y;
        sum += v_f32_ld_tnsr_b(i, a);
      }
      v_f32_st_tnsr(o, out, sum);
    }
  }
}
