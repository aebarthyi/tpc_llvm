// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -emit-llvm -O1 -main-function main_entry %s -o - | FileCheck %s

void main_entry(int arg0, tensor ifm, tensor ofm, int dest, tensor t1) {
}
// CHECK: define dso_local void @main_entry(i32 noundef %arg0, i32 noundef %dest) local_unnamed_addr #0 {
