; RUN: llc -mtriple=tpc -mcpu=gaudi2 -O2 %s -o - | FileCheck %s

; SEL takes a scalar or immediate only as its second compare operand and its
; false value, so the VSELECT combine moves a splat there. Swapping the
; values must invert the condition and swapping the compared operands must
; swap it; the combine swapped it in both cases, so `a < b ? 7 : x` became
; `a > b ? x : 7`, wrong where a == b (Habana's compiler does the same), and
; with a splat in both values it swapped them back and forth forever.

; a < b ? 7 : x  ->  a >= b ? x : 7
; CHECK-DAG: sel_geq.i32 %V{{[0-9]+}}, [[A:%V[0-9]+]], [[B:%V[0-9]+]], [[X:%V[0-9]+]], 0x7
; a == b ? 7 : x  ->  a != b ? x : 7
; CHECK-DAG: sel_neq.i32 %V{{[0-9]+}}, [[A]], [[B]], [[X]], 0x7
; 1 < b ? 7 : x  ->  b <= 1 ? x : 7
; CHECK-DAG: sel_leq.i32 %V{{[0-9]+}}, [[B]], 0x1, [[X]], %V{{[0-9]+}}
; a <u b ? 7 : x  ->  a >=u b ? x : 7
; CHECK-DAG: sel_geq.u32 %V{{[0-9]+}}, [[A]], [[B]], [[X]], 0x7
; a < b ? 7 : 9 stays as it is
; CHECK-DAG: sel_less.i32 %V{{[0-9]+}}, [[A]], [[B]], %V{{[0-9]+}}, 0x9

target triple = "tpc"

define void @main(i32 %unused) {
  %a = call <64 x i32> @llvm.tpc.ld.tnsr.v64i32.i1(<5 x i32> zeroinitializer, i8 0, i32 0, <64 x i32> undef, i1 true, i1 false)
  %b = call <64 x i32> @llvm.tpc.ld.tnsr.v64i32.i1(<5 x i32> <i32 0, i32 1, i32 0, i32 0, i32 0>, i8 0, i32 0, <64 x i32> undef, i1 true, i1 false)
  %x = call <64 x i32> @llvm.tpc.ld.tnsr.v64i32.i1(<5 x i32> <i32 0, i32 2, i32 0, i32 0, i32 0>, i8 0, i32 0, <64 x i32> undef, i1 true, i1 false)
  %lt = icmp slt <64 x i32> %a, %b
  %r0 = select <64 x i1> %lt, <64 x i32> splat (i32 7), <64 x i32> %x
  call void @llvm.tpc.st.tnsr.v64i32(<5 x i32> zeroinitializer, i8 1, <64 x i32> %r0, i32 0, i1 true, i1 false)
  %eq = icmp eq <64 x i32> %a, %b
  %r1 = select <64 x i1> %eq, <64 x i32> splat (i32 7), <64 x i32> %x
  call void @llvm.tpc.st.tnsr.v64i32(<5 x i32> <i32 0, i32 1, i32 0, i32 0, i32 0>, i8 1, <64 x i32> %r1, i32 0, i1 true, i1 false)
  %gt1 = icmp slt <64 x i32> splat (i32 1), %b
  %r2 = select <64 x i1> %gt1, <64 x i32> splat (i32 7), <64 x i32> %x
  call void @llvm.tpc.st.tnsr.v64i32(<5 x i32> <i32 0, i32 2, i32 0, i32 0, i32 0>, i8 1, <64 x i32> %r2, i32 0, i1 true, i1 false)
  %ult = icmp ult <64 x i32> %a, %b
  %r3 = select <64 x i1> %ult, <64 x i32> splat (i32 7), <64 x i32> %x
  call void @llvm.tpc.st.tnsr.v64i32(<5 x i32> <i32 0, i32 3, i32 0, i32 0, i32 0>, i8 1, <64 x i32> %r3, i32 0, i1 true, i1 false)
  %r4 = select <64 x i1> %lt, <64 x i32> splat (i32 7), <64 x i32> splat (i32 9)
  call void @llvm.tpc.st.tnsr.v64i32(<5 x i32> <i32 0, i32 4, i32 0, i32 0, i32 0>, i8 1, <64 x i32> %r4, i32 0, i1 true, i1 false)
  ret void
}

declare <64 x i32> @llvm.tpc.ld.tnsr.v64i32.i1(<5 x i32>, i8, i32, <64 x i32>, i1, i1)
declare void @llvm.tpc.st.tnsr.v64i32(<5 x i32>, i8, <64 x i32>, i32, i1, i1)
