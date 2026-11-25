// RUN: %clang_cc1 -fsyntax-only -std=rc99 -triple tpc-none-none -verify %s
// RUN: %clang_cc1 -fsyntax-only -x c++ -std=rc++ -triple tpc-none-none -verify %s

// A division whose operand has an error in it (here a call with an undeclared
// argument, which keeps the call's type) is value-dependent. The RC99 checks
// after parsing evaluated it to see whether it is a constant, and that
// asserts on a dependent expression: wright's lq/syn_qdq.c and syn_qmm.c
// crashed the assertions build this way. They now skip such expressions.

static inline int f(int x) { return x + 1; }

void main(int a) {
  int b = f(undeclared_x) / 4;  // expected-error{{use of undeclared identifier 'undeclared_x'}}
  int c = a % f(undeclared_y);  // expected-error{{use of undeclared identifier 'undeclared_y'}}
}
