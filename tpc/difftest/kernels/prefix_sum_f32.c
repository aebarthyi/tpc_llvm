// Running sum along dim 1: out[y] = a[0] + ... + a[y]. A loop-carried value
// that is also stored every iteration.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32 256 12 3
// DIFFTEST: out f32 256 12 3
// DIFFTEST: index-space 4 1 3

void main(tensor a, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  const int rows = get_dim_size(a, 1);
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      float64 run = 0.0f;
      for (int y = 0; y < rows; y++) {
        i[1] = y;
        run = run + v_f32_ld_tnsr_b(i, a);
        v_f32_st_tnsr(i, out, run);
      }
    }
  }
}
