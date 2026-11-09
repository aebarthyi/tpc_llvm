// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu goya -DOP=/ %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi -DOP=/ %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudib -DOP=/ %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu greco -DOP=/ %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi2 -DOP=/ %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi -DOP=%% %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi2 -DOP=%% %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi -DOP=/ -DUNSIGNED %s -o - | FileCheck %s
// RUN: %codegen -S -O1 -triple tpc-none-none -std=rc99 -target-cpu gaudi -DOP=%% -DUNSIGNED %s -o - | FileCheck %s

#ifdef UNSIGNED
#define VEC uint64
#define LD v_u32_ld_tnsr_b
#define ST v_u32_st_tnsr
#else
#define VEC int64
#define LD v_i32_ld_tnsr_b
#define ST v_i32_st_tnsr
#endif

void main(tensor a, tensor b, tensor out) {
  int5 i = {0, 0, 0, 0, 0};
  VEC n = LD(i, a);
  VEC d = LD(i, b);
  ST(i, out, n OP d);
}

// The compare takes a real mask as income. Gaudi cannot copy a vector register to
// a mask, and the other targets have no reason to.

// CHECK-LABEL: main:
// CHECK-NOT:   mov{{.*}}%VP{{[0-9]+}}, %V{{[0-9]+}}
// CHECK:       mov %VP{{[0-9]+}}, 0x0
// CHECK-NOT:   mov{{.*}}%VP{{[0-9]+}}, %V{{[0-9]+}}
// CHECK:       cmp_geq.u32 %VP{{[0-9]+}}, %V{{[0-9]+}}, %V{{[0-9]+}}, %VP{{[0-9]+}}
// CHECK-NOT:   mov{{.*}}%VP{{[0-9]+}}, %V{{[0-9]+}}
// CHECK:       st_tnsr
