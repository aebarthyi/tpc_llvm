// Sums, a difference and a product taken only for some rows. Each `if`
// becomes `select c, s op x, s`, which TPCSelectIntoFOp turns into
// `s op select c, x, identity` (-0.0 to add, 0.0 to subtract, 1.0 to
// multiply), as Habana's compiler does; the results must match it bit for
// bit.
//
// DIFFTEST: arch gaudi2 gaudi3
// DIFFTEST: in  f32 64 48
// DIFFTEST: in  f32 64 48 range=0.9,1.1
// DIFFTEST: out f32 64 16
// DIFFTEST: index-space 1 4

#pragma clang fp contract(off)

void main(tensor x, tensor y, tensor out) {
  const int5 start = get_index_space_offset(), end = start + get_index_space_size();
  const int rows = get_dim_size(x, 1);
  for (int col = start[1]; col < end[1]; ++col) {
    float64 sum = 0.0f, squares = 0.0f, diff = 0.0f, prod = 1.0f;
    for (int k = 0; k < rows; ++k) {
      const float64 v = v_f32_ld_tnsr_b((int5){0, k}, x);
      const float64 w = v_f32_ld_tnsr_b((int5){0, k}, y);
      sum += v;
      if ((k & 3) == col)
        squares += v * v;
      if (k % 3 == col)
        diff -= v;
      if ((k & 1) == (col & 1))
        prod *= w;
    }
    v_f32_st_tnsr((int5){0, 4 * col}, out, sum);
    v_f32_st_tnsr((int5){0, 4 * col + 1}, out, squares);
    v_f32_st_tnsr((int5){0, 4 * col + 2}, out, diff);
    v_f32_st_tnsr((int5){0, 4 * col + 3}, out, prod);
  }
}
