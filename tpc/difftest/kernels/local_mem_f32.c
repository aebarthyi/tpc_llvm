// Vector local memory: stage a column of 8 vectors in VLM, then combine each
// row with its mirror: out[y] = a[y] - a[7 - y].
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  f32 256 8 3
// DIFFTEST: out f32 256 8 3
// DIFFTEST: index-space 4 1 3

#define VEC_BYTES 256

void main(tensor a, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      for (int y = 0; y < 8; y++) {
        i[1] = y;
        v_f32_st_l_v(y * VEC_BYTES, v_f32_ld_tnsr_b(i, a));
      }
      for (int y = 0; y < 8; y++) {
        i[1] = y;
        float64 mine = v_f32_ld_l_v_b(y * VEC_BYTES);
        float64 mirror = v_f32_ld_l_v_b((7 - y) * VEC_BYTES);
        v_f32_st_tnsr(i, out, mine - mirror);
      }
    }
  }
}
