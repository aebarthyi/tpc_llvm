// Dim 0 (100) is not a multiple of the vector width: the last vector reads
// padding and its out-of-bounds lanes must not be stored.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: in  f32 100 3 5
// DIFFTEST: out f32 100 3 5
// DIFFTEST: index-space 2 3 5

void main(tensor a, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
        i[0] = x;
        v_f32_st_tnsr(i, out, v_f32_ld_tnsr_b(i, a) * 2.0f + 1.0f);
      }
    }
  }
}
