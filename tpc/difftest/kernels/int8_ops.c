// 8-bit lanes (256 per vector): u8 saturating add and i8 wrapping subtract.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  u8 1024 2 2
// DIFFTEST: in  u8 1024 2 2
// DIFFTEST: in  i8 1024 2 2
// DIFFTEST: in  i8 1024 2 2
// DIFFTEST: out u8 1024 2 2
// DIFFTEST: out i8 1024 2 2
// DIFFTEST: index-space 4 2 2

void main(tensor ua, tensor ub, tensor sa, tensor sb, tensor uout, tensor sout) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0] * 256; x < end[0] * 256; x += 256) {
        i[0] = x;
        uchar256 u = v_u8_add_b(v_u8_ld_tnsr_b(i, ua), v_u8_ld_tnsr_b(i, ub), SW_SAT);
        char256 s = v_i8_sub_b(v_i8_ld_tnsr_b(i, sa), v_i8_ld_tnsr_b(i, sb));
        v_u8_st_tnsr(i, uout, u);
        v_i8_st_tnsr(i, sout, s);
      }
    }
  }
}
