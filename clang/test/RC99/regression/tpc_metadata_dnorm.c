// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=OFF
// RUN: %tpc_clang -O1 -c -x c -march=gaudi2 -tpc-dnorm %s -o %t.o && %metadata_disasm --mcpu=gaudi2 %t.o | FileCheck %s --check-prefix=ON

void main(tensor out) {
  int5 c = {0};
  float64 v = {0};
  v_f32_st_tnsr(c, out, v);
}

// OFF:      targetSpecific: DB 0
// OFF-NEXT: dnorm: DB 0
// ON:       targetSpecific: DB 0
// ON-NEXT:  dnorm: DB 1
