// All five tensor dimensions split across the index space.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32 128 3 2 2 3
// DIFFTEST: in  f32 128 3 2 2 3
// DIFFTEST: out f32 128 3 2 2 3
// DIFFTEST: index-space 2 3 2 2 3

void main(tensor a, tensor b, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int f = start[4]; f < end[4]; f++) {
    i[4] = f;
    for (int w = start[3]; w < end[3]; w++) {
      i[3] = w;
      for (int z = start[2]; z < end[2]; z++) {
        i[2] = z;
        for (int y = start[1]; y < end[1]; y++) {
          i[1] = y;
          for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
            i[0] = x;
            float64 va = v_f32_ld_tnsr_b(i, a);
            float64 vb = v_f32_ld_tnsr_b(i, b);
            v_f32_st_tnsr(i, out, va * (float)(f + 1) - vb * (float)(w + z + y));
          }
        }
      }
    }
  }
}
