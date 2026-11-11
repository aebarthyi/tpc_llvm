// Scalar absolute value, from the intrinsic and from a ternary. The rows run from
// INT_MIN to INT_MAX. On INT_MIN the result depends on whether the compiler uses
// abs.i32, which saturates, so -O levels can differ. Habana's compiler is the
// reference.
//
// DIFFTEST: arch gaudi gaudi2 gaudi3
// DIFFTEST: out i32 256 6 1
// DIFFTEST: out i32 256 6 1
// DIFFTEST: index-space 4 6 1
// DIFFTEST: scalar i32 -2147483648
// DIFFTEST: scalar u32 858993459

void main(tensor from_intrinsic, tensor from_ternary, int first, unsigned step) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int y = start[1]; y < end[1]; y++) {
    i[1] = y;
    const int v = first + y * step; // unsigned, so this may wrap
    const int a = s_i32_abs(v);
    const int b = v < 0 ? -v : v;
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      v_i32_st_tnsr(i, from_intrinsic, a);
      v_i32_st_tnsr(i, from_ternary, b);
    }
  }
}
