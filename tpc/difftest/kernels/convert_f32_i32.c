// f32 <-> i32 conversions with explicit rounding; two outputs.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32 256 6 3 range=-1000,1000
// DIFFTEST: out i32 256 6 3
// DIFFTEST: out f32 256 6 3
// DIFFTEST: index-space 4 6 3

void main(tensor a, tensor as_int, tensor round_trip) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
        i[0] = x;
        float64 v = v_f32_ld_tnsr_b(i, a);
        int64 n = convert_float64_to_int64(v, SW_RHNE);
        int64 t = convert_float64_to_int64(v, SW_RZ);
        v_i32_st_tnsr(i, as_int, n);
        v_f32_st_tnsr(i, round_trip, convert_int64_to_float64(t, 0) - v);
      }
    }
  }
}
