// Elementwise f32 add over a 3-D tensor; one index-space member per vector.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32 256 6 3
// DIFFTEST: in  f32 256 6 3
// DIFFTEST: out f32 256 6 3
// DIFFTEST: index-space 4 6 3

void main(tensor a, tensor b, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 c = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    c[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      c[1] = y;
      for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
        c[0] = x;
        float64 va = v_f32_ld_tnsr_b(c, a, 0, 0, 1, 0);
        float64 vb = v_f32_ld_tnsr_b(c, b, 0, 0, 1, 0);
        v_f32_st_tnsr(c, out, v_f32_add_b(va, vb, 0, 0, 1, 0), 0, 1, 0);
      }
    }
  }
}
