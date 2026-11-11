// tpclib special functions: exp(a) and log(b) (b strictly positive).
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  f32 256 6 3 range=-20,20
// DIFFTEST: in  f32 256 6 3 range=0.001,1000
// DIFFTEST: out f32 256 6 3
// DIFFTEST: out f32 256 6 3
// DIFFTEST: index-space 4 6 3

void main(tensor a, tensor b, tensor exp_out, tensor log_out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
        i[0] = x;
        v_f32_st_tnsr(i, exp_out, v_exp_f32(v_f32_ld_tnsr_b(i, a)));
        v_f32_st_tnsr(i, log_out, v_log_f32(v_f32_ld_tnsr_b(i, b)));
      }
    }
  }
}
