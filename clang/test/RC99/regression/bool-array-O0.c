// RUN: %tpc_clang -march=gaudi2 -O0 -S %s -o - | FileCheck %s
// RUN: %tpc_clang -march=gaudi2 -O2 -S %s -o - | FileCheck --check-prefix=O2 %s

// Clang aligns a bool64 (or bool128, bool256) access to its C size, 8 bytes,
// but the predicate is stored in a whole 256-byte VLM row, and an array of
// them has a stride of 256. At -O0 nothing raised the alignment before
// instruction selection, which then expanded the store through a stack slot
// and failed: "Cannot select: v256i1 = load ... from %stack.0". 1cat's
// deepseek_v41_qk_flat_reuse_softmax_gaudi2 failed at -O0 this way.
// TPCMemoryAlign gives every vector access in private and vector local
// memory its type's alignment, as Habana's compiler does; at -O0 this kernel
// then compiles to the same code as Habana's.

void main(tensor in, tensor out, int k) {
  bool64 valid[4];
  __local__ static bool128 shared[3];
  const float64 x = v_f32_ld_tnsr_b((int5){0}, in);
  valid[k] = v_f32_cmp_grt_b(x, 0.0f);
  shared[k] = shared[k + 1];
  const float64 y = v_f32_mov_vb(x, 0, 0.0f, valid[k + 1], 0);
  v_f32_st_tnsr((int5){0}, out, y);
}

// valid[k], shared[k + 1], shared[k], valid[k + 1]:
// CHECK: st_l_v %S{{[0-9]+}}, %VP{{[0-9]+}}
// CHECK: ld_l_v %VP{{[0-9]+}}, %S{{[0-9]+}}
// CHECK: st_l_v %S{{[0-9]+}}, %VP{{[0-9]+}}
// CHECK: ld_l_v %VP{{[0-9]+}}, %S{{[0-9]+}}
// CHECK: halt

// O2: halt
