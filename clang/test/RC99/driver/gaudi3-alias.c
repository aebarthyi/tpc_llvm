// gaudi3, Habana's name for the chip, selects the doron1 target everywhere.
// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -target-cpu gaudi3 -E -dM %s -o - | FileCheck --check-prefix=MACROS %s
// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -target-cpu doron1 -E -dM %s -o - | FileCheck --check-prefix=MACROS %s
// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -target-cpu gaudi3 -O1 -emit-llvm %s -o - | FileCheck --check-prefix=IR %s
// RUN: %tpc_clang -march=gaudi3 -O2 -c %s -o %t.o
// RUN: llvm-objdump -d --triple=tpc --mcpu=gaudi3 %t.o | FileCheck --check-prefix=DIS %s
// RUN: echo halt | llvm-mc -triple=tpc -mcpu=gaudi3 -filetype=obj -o /dev/null

void main(int dest) {
  *(int __local *)dest = 1;
}

// Habana's compiler defines __gaudi3__ and __gaudi3_plus__; the fork's headers
// test __doron1__.
// MACROS-DAG: #define __doron1__ 1
// MACROS-DAG: #define __gaudi3__ 1
// MACROS-DAG: #define __gaudi3_plus__ 1

// IR: "target-cpu"="doron1"

// DIS: TPC architecture: doron1
// DIS: halt
