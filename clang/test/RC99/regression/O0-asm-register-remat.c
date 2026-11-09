// RUN: %tpc_clang -c -O0 -march=gaudi2 %s -o %t.o
// RUN: %tpc_clang -c -O0 -march=gaudi2 -DVECTOR %s -o %t.o

// At -O0 the register coalescer rematerializes the undefined value of an asm register
// variable into its physical register, as IMPLICIT_DEF when the type is a vector.

void main(int x) {
#ifdef VECTOR
  register float64 result __asm__("v3");
  register float64 source __asm__("v6");
  __asm volatile ("nop; nop; mov.f32 %0, %1" :"=S" (result): "s" (source):);
#else
  register int result __asm__("s11");
  register int source __asm__("s20");
  __asm volatile ("nop; mov.f32 %0, %1" :"=s" (result): "s" (source):);
#endif
}
