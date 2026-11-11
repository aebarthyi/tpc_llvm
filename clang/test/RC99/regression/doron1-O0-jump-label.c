// RUN: %tpc_clang -S -O0 -march=doron1 %s -o - | FileCheck %s
// RUN: %tpc_clang -c -O0 -march=doron1 %s -o %t.o

// On doron1 the instruction after a jump is its delay slot, which -O0 fills
// with a NOP. The jump is then not the last instruction of its block, but the
// label of the block it jumps to must still be printed (TPCAsmPrinter.cpp);
// otherwise the object fails with "Undefined temporary symbol".

void main(int dest, int n) {
  int __local *p = (int __local *)dest;
  for (int i = 0; i < n; i++)
    p[i] = i;
}

// CHECK:      jmpr .LBB0_2{{$}}
// CHECK-NEXT: nop
// CHECK-NEXT: .LBB0_2: {{.*}}// %for.body
// CHECK:      jmpr .LBB0_3{{$}}
// CHECK-NEXT: nop
// CHECK-NEXT: .LBB0_3: {{.*}}// %for.inc
