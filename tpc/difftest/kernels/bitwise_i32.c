// i32 bitwise logic and wrapping add: out = ((a & b) ^ (~a | c)) + a.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  i32 256 6 3
// DIFFTEST: in  i32 256 6 3
// DIFFTEST: in  i32 256 6 3
// DIFFTEST: out i32 256 6 3
// DIFFTEST: index-space 4 6 3

void main(tensor a, tensor b, tensor c, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
        i[0] = x;
        int64 va = v_i32_ld_tnsr_b(i, a);
        int64 vb = v_i32_ld_tnsr_b(i, b);
        int64 vc = v_i32_ld_tnsr_b(i, c);
        int64 r = (va & vb) ^ (~va | vc);
        v_i32_st_tnsr(i, out, v_i32_add_b(r, va));
      }
    }
  }
}
