// RUN: %clang_cc1 -fsyntax-only -std=rc99 -triple tpc-none-none -verify=warn %s
// RUN: %clang_cc1 -fsyntax-only -std=rc99 -triple tpc-none-none -Werror -verify=err %s
// RUN: %clang_cc1 -fsyntax-only -std=rc99 -triple tpc-none-none -Werror=incompatible-pointer-types -verify=err %s
// RUN: %clang_cc1 -fsyntax-only -std=rc99 -triple tpc-none-none -Werror -Wno-error=incompatible-pointer-types -verify=warn %s
// RUN: %clang_cc1 -fsyntax-only -std=rc99 -triple tpc-none-none -Wno-incompatible-pointer-types -verify=none %s

// Clang 22 makes incompatible pointer types an error in C by default. RC99
// keeps the warning, which -Werror still turns into an error.

// none-no-diagnostics

void main(int dest) {
  float64 __local *ptr = (int64 __local *)dest; // warn-warning{{incompatible pointer types}} err-error{{incompatible pointer types}}
}
