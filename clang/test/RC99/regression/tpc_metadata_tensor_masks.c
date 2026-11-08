// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DPARTIAL %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=ONE
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DPARTIAL_REG %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=ALL
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DSQZ %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=ONE
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DSQZ_REG %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=ALL
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DHW_REG %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=ONE
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DPLAIN %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=NONE
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DLOW %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=NONE
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DGLOBAL %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=GLOBAL
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DGLOBAL_REG %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=GLOBAL_REG
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DPREDICATE_LOAD %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=PREDICATE_LOAD
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -DVECTOR_LOAD %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=VECTOR_LOAD

void main(tensor a, tensor b, tensor out, int reg, int vpp) {
  int5 c = {0};
  float64 v = {0};
#if defined(PARTIAL)
  v_f32_st_tnsr_partial(c, out, v, 4, 0);
#elif defined(PARTIAL_REG)
  v_f32_st_tnsr_partial(c, reg, v, 4, 0);
#elif defined(SQZ)
  bool256 vp = *(bool256 __local *)vpp;
  v_f32_st_tnsr_sqz(c, out, 0, v, 0, vp, 0);
#elif defined(SQZ_REG)
  bool256 vp = *(bool256 __local *)vpp;
  v_f32_st_tnsr_sqz(c, reg, 0, v, 0, vp, 0);
#elif defined(HW_REG)
  squeeze_cntr s = 0;
  st_tnsr_s_hwr(c, out, s, 0, 1, 0);
#elif defined(PLAIN)
  v_f32_st_tnsr(c, out, v);
#elif defined(LOW)
  v_f32_st_tnsr_low(c, out, v);
#elif defined(GLOBAL)
  int __global *p = gen_addr(c, a, 0, 0, 1, 0);
  *p = 7;
  int __global *q = gen_addr(c, b, 0, 0, 1, 0);
  *p = *q;
#elif defined(GLOBAL_REG)
  int __global *p = gen_addr(c, reg, 0, 0, 1, 0);
  *p = 7;
#elif defined(PREDICATE_LOAD)
  __global void *p = gen_addr(c, a, 0, 0, 1, 0);
  v_f32_st_tnsr(c, out, v, 0, s_i1_ld_g(p), 0);
#elif defined(VECTOR_LOAD)
  __global void *p = gen_addr(c, a, 0, 0, 1, 0);
  v_f32_st_tnsr(c, out, v_f32_ld_g(p));
#endif
}

// ONE-NOT:  partialStore[0]
// ONE:      partialStore[2]: DB 1
// ONE-NOT:  partialStore

// ALL:      partialStore[0]: DB 1
// ALL:      partialStore[15]: DB 1

// NONE-NOT: partialStore

// GLOBAL:     scalarLd[1]: DB 1
// GLOBAL-NOT: scalarLd
// GLOBAL:     genAddrTensorMask[0]: DB 1
// GLOBAL-NEXT: genAddrTensorMask[1]: DB 1
// GLOBAL:     scalarSt[0]: DB 1
// GLOBAL-NOT: scalarSt

// GLOBAL_REG-NOT: genAddrTensorMask
// GLOBAL_REG:     scalarSt[0]: DB 1
// GLOBAL_REG:     scalarSt[15]: DB 1

// PREDICATE_LOAD: scalarLd[0]: DB 1
// PREDICATE_LOAD: genAddrTensorMask[0]: DB 1

// VECTOR_LOAD-NOT: scalarLd
// VECTOR_LOAD:     genAddrTensorMask[0]: DB 1
