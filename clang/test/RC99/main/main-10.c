// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -emit-llvm -main-function qqq -O1 %s -o - | FileCheck %s

void qqq(int I){
}
// CHECK: define dso_local void @qqq(i32 noundef %I) {{.*}} {
// CHECK: ret void
