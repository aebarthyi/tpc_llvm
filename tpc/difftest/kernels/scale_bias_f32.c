// out = nearbyint(|a| * scale + bias) with scalar kernel parameters.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32 256 6 3
// DIFFTEST: out f32 256 6 3
// DIFFTEST: index-space 4 6 3
// DIFFTEST: scalar f32 1.75
// DIFFTEST: scalar f32 -0.5

void main(tensor a, tensor out, float scale, float bias) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
        i[0] = x;
        float64 v = v_f32_abs_b(v_f32_ld_tnsr_b(i, a));
        v = v_f32_nearbyint_b(v * scale + bias);
        v_f32_st_tnsr(i, out, v);
      }
    }
  }
}
