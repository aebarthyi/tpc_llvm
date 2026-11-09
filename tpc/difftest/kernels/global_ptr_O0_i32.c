// A global pointer made in one block and used in a later one. At -O0 it has to stay
// in a register, since nothing can load or store it.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  i32 256 6 1 range=-1000,1000
// DIFFTEST: out i32 256 6 1
// DIFFTEST: index-space 4 6 1
// DIFFTEST: cflags -O0
void main(tensor in, tensor out) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};
  for (int y = start[1]; y < end[1]; y++) {
    i[0] = start[0] * 64;
    i[1] = y;
    int __global *p = gen_addr(i, in, 0, 0, 1, 0);
    int v = y;
    if (y & 1)
      v = s_i32_ld_g(p, 0, v, 0, 1);
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      v_i32_st_tnsr(i, out, v);
    }
  }
}
