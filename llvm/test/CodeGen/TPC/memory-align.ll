; RUN: opt -mtriple=tpc -mcpu=gaudi2 -passes=tpcmemalign -S %s | FileCheck %s

; A vector load or store in private memory (address space 0) or vector local
; memory (address space 2) gets its type's alignment, 256: clang gives a
; bool64 its C alignment, 8, and the backend cannot select a <256 x i1>
; access aligned to less than 256. Scalars, int5, scalar local memory
; (address space 1) and global memory (address space 3) keep theirs.

; CHECK-LABEL: @private(
; CHECK:         store <256 x i1> %m, ptr %p, align 256
; CHECK:         load <256 x i1>, ptr %q, align 256
; CHECK:         store <64 x float> %v, ptr %p, align 256
; CHECK:         store <5 x i32> %c, ptr %r, align 4
; CHECK:         store i32 %i, ptr %r, align 4
define <256 x i1> @private(ptr %p, ptr %q, ptr %r, <256 x i1> %m, <64 x float> %v, <5 x i32> %c, i32 %i) {
  store <256 x i1> %m, ptr %p, align 8
  %l = load <256 x i1>, ptr %q, align 8
  store <64 x float> %v, ptr %p, align 4
  store <5 x i32> %c, ptr %r, align 4
  store i32 %i, ptr %r, align 4
  ret <256 x i1> %l
}

; CHECK-LABEL: @vector_local(
; CHECK:         store <256 x i1> %m, ptr addrspace(2) %p, align 256
; CHECK:         load <128 x bfloat>, ptr addrspace(2) %p, align 256
define <128 x bfloat> @vector_local(ptr addrspace(2) %p, <256 x i1> %m) {
  store <256 x i1> %m, ptr addrspace(2) %p, align 16
  %l = load <128 x bfloat>, ptr addrspace(2) %p, align 2
  ret <128 x bfloat> %l
}

; CHECK-LABEL: @others(
; CHECK:         store i32 %i, ptr addrspace(1) %s, align 4
; CHECK:         store <64 x float> %v, ptr addrspace(3) %g, align 4
define void @others(ptr addrspace(1) %s, ptr addrspace(3) %g, i32 %i, <64 x float> %v) {
  store i32 %i, ptr addrspace(1) %s, align 4
  store <64 x float> %v, ptr addrspace(3) %g, align 4
  ret void
}
