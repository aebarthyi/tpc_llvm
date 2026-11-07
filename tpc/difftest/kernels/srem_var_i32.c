// Signed scalar remainder by a non-constant divisor.
//
// Known fork bug (LLVM 12 baseline): ISel asserts "unimplemented operand" in
// TPCTargetLowering::LowerOperation for ISD::SREM. Habana's compiler handles
// it. Expected result until fixed: KNOWN.
//
// DIFFTEST: arch gaudi gaudi2
// DIFFTEST: in  i32 256 6 1
// DIFFTEST: out i32 256 6 1
// DIFFTEST: index-space 4 6 1
// DIFFTEST: scalar i32 -1234567

void main(tensor a, tensor out, int n) {
  const int5 start = get_index_space_offset();
  const int5 end = get_index_space_size() + start;
  int5 i = {0, 0, 0, 0, 0};

  for (int y = start[1]; y < end[1]; y++) {
    i[1] = y;
    int r = n % (y + 3);
    for (int x = start[0] * 64; x < end[0] * 64; x += 64) {
      i[0] = x;
      v_i32_st_tnsr(i, out, v_i32_ld_tnsr_b(i, a) + r);
    }
  }
}
