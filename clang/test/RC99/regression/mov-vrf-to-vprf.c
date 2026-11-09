// RUN: %codegen -S -O0 -triple tpc-none-none -std=rc99 -target-cpu greco %s -o - | FileCheck %s

// At -O0 on greco this kernel copies a vector register to a mask. The copy is not
// predicated: it must not carry a scalar predicate in a vector predicate operand,
// which the instruction printer rejects.

void main(tensor in, tensor out) {
  int5 coords = {0, 0, 0, 0, 0};
  half128 income = 0;
  income = v_f16_ld_tnsr_b(coords, in, 0, income, 1, 0);
  half128 input_class = v_f16_fclass_b(income, 1, 0, 1, 0);
  half128 result = v_f16_calc_fp_special_b(input_class, input_class, 5, income, 1, 0);
  v_f16_st_tnsr(coords, out, result, 0, 1, 0);
}

// CHECK: mov %VP{{[0-9]+}}, %V{{[0-9]+}}{{$}}
