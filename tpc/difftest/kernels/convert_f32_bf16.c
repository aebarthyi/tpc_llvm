// f32 -> bf16 (round to nearest even) -> f32. One bf16 vector covers two
// f32 vectors, so dim 0 steps by 128 elements.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32  256 6 3
// DIFFTEST: out bf16 256 6 3
// DIFFTEST: out f32  256 6 3
// DIFFTEST: index-space 2 6 3

void main(tensor a, tensor narrow, tensor widened) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};
  int5 hi = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = hi[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = hi[1] = y;
      for (int x = start[0] * 128; x < end[0] * 128; x += 128) {
        i[0] = x;
        hi[0] = x + 64;
        float128 pair = {v_f32_ld_tnsr_b(i, a), v_f32_ld_tnsr_b(hi, a)};
        bfloat128 h = convert_float128_to_bfloat128(pair, SW_RHNE);
        v_bf16_st_tnsr(i, narrow, h);
        float128 back = convert_bfloat128_to_float128(h, 0);
        v_f32_st_tnsr(i, widened, back.v1);
        v_f32_st_tnsr(hi, widened, back.v2);
      }
    }
  }
}
