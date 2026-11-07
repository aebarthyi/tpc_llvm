// Elementwise bf16 add; 128 lanes per vector.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  bf16 512 4 2
// DIFFTEST: in  bf16 512 4 2
// DIFFTEST: out bf16 512 4 2
// DIFFTEST: index-space 4 4 2

void main(tensor a, tensor b, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0] * 128; x < end[0] * 128; x += 128) {
        i[0] = x;
        bfloat128 va = v_bf16_ld_tnsr_b(i, a);
        bfloat128 vb = v_bf16_ld_tnsr_b(i, b);
        v_bf16_st_tnsr(i, out, v_bf16_add_b(va, vb));
      }
    }
  }
}
