// Count, per lane, how many rows exceed a threshold: a predicated i32 add
// driven by an f32 compare.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  f32 256 20 2
// DIFFTEST: out i32 256 1 2
// DIFFTEST: index-space 4 1 2
// DIFFTEST: scalar f32 2.5

void main(tensor a, tensor out, float threshold) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  const int rows = get_dim_size(a, 1);
  int5 i = {0, 0, 0, 0, 0};
  int5 o = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = o[2] = z;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = o[0] = x;
      int64 count = 0;
      for (int y = 0; y < rows; y++) {
        i[1] = y;
        bool64 above = v_f32_cmp_grt_b(v_f32_ld_tnsr_b(i, a), threshold);
        count = v_i32_add_vb(count, 1, 0, count, above, 0);
      }
      v_i32_st_tnsr(o, out, count);
    }
  }
}
