; RUN: opt -passes=instcombine -S %s | FileCheck %s

; Asking for the FP class of a bitcast compares its bits with the bit pattern of
; infinity, so APFloat must give the 8-bit float types an 8-bit pattern.

target triple = "tpc"

; CHECK-LABEL: @f8_143(
; CHECK: fmul f8_143
define f8_143 @f8_143(i8 %x) {
  %f = bitcast i8 %x to f8_143
  %m = fmul f8_143 %f, %f
  %r = fadd f8_143 %m, %f
  ret f8_143 %r
}

; CHECK-LABEL: @f8_152(
; CHECK: fmul f8_152
define f8_152 @f8_152(i8 %x) {
  %f = bitcast i8 %x to f8_152
  %m = fmul f8_152 %f, %f
  %r = fadd f8_152 %m, %f
  ret f8_152 %r
}
