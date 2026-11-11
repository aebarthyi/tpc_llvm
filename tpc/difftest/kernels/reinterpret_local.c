// Read local memory through a differently typed pointer. The optimizer forwards
// the store to the load, so the backend sees a bitcast between value types:
// a vector bf16 -> f8 reinterpretation, and a scalar f32 -> f8 one, which goes
// through an integer truncation.
//
// DIFFTEST: arch gaudi2 gaudi3
// DIFFTEST: cflags -fno-strict-aliasing
// DIFFTEST: in  bf16 128 4 3 range=-4,4
// DIFFTEST: out f8_143 256 4 3
// DIFFTEST: out f8_143 256 4 3
// DIFFTEST: index-space 1 4 3
// DIFFTEST: scalar f32 1.5

void main(tensor in, tensor vec_out, tensor scalar_out, float value) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 src = {0, 0, 0, 0, 0};
  int5 dst = {0, 0, 0, 0, 0};

  bfloat128 __local *vec_bf16 = (bfloat128 __local *)256;
  minifloat256 __local *vec_f8 = (minifloat256 __local *)256;
  float __local *scalar_f32 = (float __local *)0;
  minifloat __local *scalar_f8 = (minifloat __local *)0;

  for (int z = start[2]; z < end[2]; z++) {
    src[2] = dst[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      src[1] = dst[1] = y;
      for (int x = start[0]; x < end[0]; x++) {
        src[0] = x * 128;
        dst[0] = x * 256;

        *vec_bf16 = v_bf16_ld_tnsr_b(src, in);
        v_f8_st_tnsr(dst, vec_out, *vec_f8);

        *scalar_f32 = value;
        minifloat256 splat = *scalar_f8;
        v_f8_st_tnsr(dst, scalar_out, splat);
      }
    }
  }
}
