// RUN: %tpc_clang -march=gaudi2 -O2 -S -ffp-contract=off %s -o - | FileCheck %s
// RUN: %tpc_clang -march=gaudi2 -O2 -S -ffp-contract=off -mllvm -tpc-enable-select-into-fop=false %s -o - | FileCheck --check-prefix=OFF %s

// The `if` becomes `select c, s + v * v, s`, with the select on the sum's
// chain: each add of the unrolled loop waited for the previous add and its
// predicated move back to the sum. TPCSelectIntoFOp makes it
// `s + select c, v * v, -0.0`, as Habana's compiler does: the square gets
// -0.0 (0x80000000) where the condition fails, and the sum is added to in
// place. 1cat's deepseek_v41_mhc_control_reuse took 1.20 times Habana's
// cycles with the select on the chain, and 1.11 without.

void main(tensor x, tensor out, int pick) {
  const int rows = get_dim_size(x, 1);
  float64 squares = 0.0f;
  for (int k = 0; k < rows; ++k) {
    const float64 v = v_f32_ld_tnsr_b((int5){0, k}, x);
    if (k == pick)
      squares += v * v;
  }
  v_f32_st_tnsr((int5){0, 0}, out, squares);
}

// CHECK:     mul.f32 [[SQ:%V[0-9]+]], [[V:%V[0-9]+]], [[V]]
// CHECK:     mov.f32 [[SQ]], -0x80000000, !%SP{{[0-9]+}}
// CHECK:     add.f32 [[S:%V[0-9]+]], [[S]], [[SQ]]
// CHECK-NOT: mov %V{{[0-9]+}}, %V{{[0-9]+}}, !%SP
// CHECK:     halt

// OFF:       add.f32 [[N:%V[0-9]+]], [[S:%V[0-9]+]], %V{{[0-9]+}}
// OFF-NEXT:  mov [[N]], [[S]], !%SP{{[0-9]+}}
