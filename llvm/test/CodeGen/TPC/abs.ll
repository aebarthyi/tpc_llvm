; RUN: llc -march=tpc -mcpu=gaudi -O2 -ignore-mem-overflow %s -o - | FileCheck %s
; RUN: llc -march=tpc -mcpu=gaudi2 -O2 -ignore-mem-overflow %s -o - | FileCheck %s

; ISD::ABS is expanded, and instruction patterns turn the expansion back into abs.
; When upstream changes the shape of that expansion, the patterns stop matching
; without any error.

target triple = "tpc"

; CHECK-LABEL: abs_i32:
; CHECK-NOT:   xor
; CHECK:       abs.i32 %S{{[0-9]+}}, %S{{[0-9]+}}
; CHECK-NOT:   xor
define dso_local void @abs_i32(i32 %a, i32 %dest) {
  %r = call i32 @llvm.abs.i32(i32 %a, i1 false)
  %p = inttoptr i32 %dest to ptr addrspace(1)
  store i32 %r, ptr addrspace(1) %p, align 4
  ret void
}

; CHECK-LABEL: abs_i32_min_poison:
; CHECK-NOT:   xor
; CHECK:       abs.i32 %S{{[0-9]+}}, %S{{[0-9]+}}
; CHECK-NOT:   xor
define dso_local void @abs_i32_min_poison(i32 %a, i32 %dest) {
  %r = call i32 @llvm.abs.i32(i32 %a, i1 true)
  %p = inttoptr i32 %dest to ptr addrspace(1)
  store i32 %r, ptr addrspace(1) %p, align 4
  ret void
}

; CHECK-LABEL: abs_i16:
; CHECK-NOT:   xor
; CHECK:       abs.i16 %S{{[0-9]+}}, %S{{[0-9]+}}
; CHECK-NOT:   xor
define dso_local void @abs_i16(i32 %a, i32 %dest) {
  %x = trunc i32 %a to i16
  %r = call i16 @llvm.abs.i16(i16 %x, i1 false)
  %p = inttoptr i32 %dest to ptr addrspace(1)
  store i16 %r, ptr addrspace(1) %p, align 2
  ret void
}

; CHECK-LABEL: abs_i8:
; CHECK-NOT:   xor
; CHECK:       abs.i8 %S{{[0-9]+}}, %S{{[0-9]+}}
; CHECK-NOT:   xor
define dso_local void @abs_i8(i32 %a, i32 %dest) {
  %x = trunc i32 %a to i8
  %r = call i8 @llvm.abs.i8(i8 %x, i1 false)
  %p = inttoptr i32 %dest to ptr addrspace(1)
  store i8 %r, ptr addrspace(1) %p, align 1
  ret void
}

; Habana's compiler leaves the negated form as shifts. It wraps on INT_MIN, while
; abs.i32 saturates, so it must not become abs.

; CHECK-LABEL: negated_abs_i32:
; CHECK-NOT:   abs.i32
; CHECK:       xor.i32
; CHECK-NOT:   abs.i32
define dso_local void @negated_abs_i32(i32 %a, i32 %dest) {
  %s = ashr i32 %a, 31
  %x = xor i32 %a, %s
  %r = sub i32 %s, %x
  %p = inttoptr i32 %dest to ptr addrspace(1)
  store i32 %r, ptr addrspace(1) %p, align 4
  ret void
}

declare i32 @llvm.abs.i32(i32, i1 immarg)
declare i16 @llvm.abs.i16(i16, i1 immarg)
declare i8 @llvm.abs.i8(i8, i1 immarg)
