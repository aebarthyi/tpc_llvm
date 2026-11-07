// Max-reduce over dim 1 with an unroll pragma and two independent
// accumulators; exercises the unroller and the scheduler/packetizer.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32 256 32 2
// DIFFTEST: out f32 256 1 2
// DIFFTEST: index-space 4 1 2

void main(tensor a, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};
  int5 j = {0, 0, 0, 0, 0};
  int5 o = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = j[2] = o[2] = z;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = j[0] = o[0] = x;
      float64 m0 = -3.0e38f;
      float64 m1 = -3.0e38f;
#pragma unroll 4
      for (int y = 0; y < 32; y += 2) {
        i[1] = y;
        j[1] = y + 1;
        m0 = v_f32_max_b(m0, v_f32_ld_tnsr_b(i, a));
        m1 = v_f32_max_b(m1, v_f32_ld_tnsr_b(j, a));
      }
      v_f32_st_tnsr(o, out, v_f32_max_b(m0, m1));
    }
  }
}
