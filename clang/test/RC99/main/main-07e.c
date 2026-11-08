// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -emit-llvm -O1 %s -o - | FileCheck %s
// CHECK: define dso_local void @main(i32 noundef %arg0, i32 noundef %arg1, i32 noundef %arg2, i32 noundef %arg3, i32 noundef %arg4, i32 noundef %arg5, i32 noundef %arg6, i32 noundef %arg7, i32 noundef %arg8, i32 noundef %arg9, i32 noundef %arg10, i32 noundef %arg11, i32 noundef %arg12, i32 noundef %arg13, i32 noundef %arg14, i32 noundef %arg15, i32 noundef %arg16, i32 noundef %arg17, i32 noundef %arg18, i32 noundef %arg19, i32 noundef %arg20, i32 noundef %arg21, i32 noundef %arg22, i32 noundef %arg23, i32 noundef %arg24, i32 noundef %arg25, i32 noundef %arg26, i32 noundef %arg27, i32 noundef %arg28, i32 noundef %arg29, i32 noundef %arg30, float noundef %float34) local_unnamed_addr #0 { 

#pragma tpc_printf (enable)
void main(tensor T0,
          tensor T1,
          tensor T2,
          tensor T3,
          tensor T4,
          tensor T5,
//          tensor T6,
//          tensor T7,
          int arg0, int arg1, int arg2, int arg3, int arg4,
  int arg5, int arg6, int arg7, int arg8, int arg9,
  int arg10, int arg11, int arg12, int arg13, int arg14,
  int arg15, int arg16, int arg17, int arg18, int arg19,
  int arg20, int arg21, int arg22, int arg23, int arg24,
  int arg25, int arg26, int arg27, int arg28, int arg29,
  int arg30, float float34, int arg31) {
}
