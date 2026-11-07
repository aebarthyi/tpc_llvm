// tpclib activations: sigmoid(a) and tanh(a).
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  f32 256 6 3 range=-12,12
// DIFFTEST: out f32 256 6 3
// DIFFTEST: out f32 256 6 3
// DIFFTEST: index-space 4 6 3

void main(tensor a, tensor sig_out, tensor tanh_out) {
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
        v_f32_st_tnsr(i, sig_out, v_sigmoid_f32(v));
        v_f32_st_tnsr(i, tanh_out, v_tanh_f32(v));
      }
    }
  }
}
