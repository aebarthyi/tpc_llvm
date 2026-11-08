// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -emit-llvm -O1 %s -o - | FileCheck %s

void main(int dest, tensor t0, float arg2, tensor t1, tensor t2) {
  *(int volatile __local *)dest = t0;
  *(int volatile __local *)dest = t1;
  *(int volatile __local *)dest = t2;
}

// CHECK: define dso_local void @main(i32 noundef %dest, float noundef %arg2) local_unnamed_addr #0 {
// CHECK: store volatile i32 0, ptr addrspace(1) %{{[0-9]+}}
// CHECK: store volatile i32 1, ptr addrspace(1) %{{[0-9]+}}
// CHECK: store volatile i32 2, ptr addrspace(1) %{{[0-9]+}}
