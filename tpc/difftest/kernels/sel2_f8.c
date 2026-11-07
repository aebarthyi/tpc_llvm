// SEL2 on f8 with its two-vector result written to and read back from vector
// local memory, which exercises storing a whole 512 x f8 register pair.
//
// DIFFTEST: arch gaudi2
// DIFFTEST: in  f8_143 256 4 3
// DIFFTEST: in  f8_143 256 4 3
// DIFFTEST: in  f8_143 256 4 3
// DIFFTEST: in  f8_143 256 4 3
// DIFFTEST: out f8_143 256 4 3
// DIFFTEST: out f8_143 256 4 3
// DIFFTEST: index-space 1 4 3

typedef struct _minifloat256_pair_t minifloat256_minifloat256;

void main(tensor a, tensor b, tensor c, tensor d, tensor out1, tensor out2) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  volatile minifloat256_minifloat256 __local *pair =
      (minifloat256_minifloat256 __local *)0;

  for (int z = start[2]; z < end[2]; z++) {
    i[2] = z;
    for (int y = start[1]; y < end[1]; y++) {
      i[1] = y;
      for (int x = start[0]; x < end[0]; x++) {
        i[0] = x * 256;

        minifloat256 va = v_f8_ld_tnsr_b(i, a);
        minifloat256 vb = v_f8_ld_tnsr_b(i, b);
        minifloat256 vc = v_f8_ld_tnsr_b(i, c);
        minifloat256 vd = v_f8_ld_tnsr_b(i, d);

        minifloat256_minifloat256 income = *pair;
        *pair = v_f8_sel2_geq_f8_b(va, vb, vc, vd, 0, income, 1, 0);

        minifloat256_minifloat256 result = *pair;
        v_f8_st_tnsr(i, out1, result.v1);
        v_f8_st_tnsr(i, out2, result.v2);
      }
    }
  }
}
