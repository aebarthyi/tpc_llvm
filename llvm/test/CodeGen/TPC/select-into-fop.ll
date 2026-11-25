; RUN: opt -mtriple=tpc -mcpu=gaudi2 -passes=tpc-select-into-fop -S %s | FileCheck %s
; RUN: opt -mtriple=tpc -mcpu=gaudi2 -passes=tpc-select-into-fop -tpc-enable-select-into-fop=false -S %s | FileCheck %s --check-prefix=OFF

; `select c, s op m, s` becomes `s op select c, m, identity`, which takes the
; select off s's chain, also where s may be a NaN (as an argument or a loop's
; sum may), which InstCombine does not do.

; CHECK-LABEL: @fadd(
; CHECK-NEXT:    [[SEL:%.*]] = select i1 %c, <64 x float> %m, <64 x float> splat (float -0.000000e+00)
; CHECK-NEXT:    [[R:%.*]] = fadd <64 x float> %s, [[SEL]]
; CHECK-NEXT:    ret <64 x float> [[R]]
; OFF-LABEL:   @fadd(
; OFF-NEXT:      fadd
; OFF-NEXT:      select
define <64 x float> @fadd(i1 %c, <64 x float> %s, <64 x float> %m) {
  %a = fadd <64 x float> %s, %m
  %r = select i1 %c, <64 x float> %a, <64 x float> %s
  ret <64 x float> %r
}

; The sum as the add's second operand, the select's arms swapped.
; CHECK-LABEL: @fadd_swapped(
; CHECK-NEXT:    [[SEL:%.*]] = select i1 %c, float -0.000000e+00, float %m
; CHECK-NEXT:    [[R:%.*]] = fadd float %s, [[SEL]]
; CHECK-NEXT:    ret float [[R]]
define float @fadd_swapped(i1 %c, float %s, float %m) {
  %a = fadd float %m, %s
  %r = select i1 %c, float %s, float %a
  ret float %r
}

; CHECK-LABEL: @fsub(
; CHECK-NEXT:    [[SEL:%.*]] = select i1 %c, float %m, float 0.000000e+00
; CHECK-NEXT:    [[R:%.*]] = fsub float %s, [[SEL]]
define float @fsub(i1 %c, float %s, float %m) {
  %a = fsub float %s, %m
  %r = select i1 %c, float %a, float %s
  ret float %r
}

; CHECK-LABEL: @fmul(
; CHECK-NEXT:    [[SEL:%.*]] = select i1 %c, float %m, float 1.000000e+00
; CHECK-NEXT:    [[R:%.*]] = fmul float %s, [[SEL]]
define float @fmul(i1 %c, float %s, float %m) {
  %a = fmul float %s, %m
  %r = select i1 %c, float %a, float %s
  ret float %r
}

; Left alone: m - s (no identity on the left), an add used elsewhere too,
; and a constant m (a select of two constants).
; CHECK-LABEL: @kept(
; CHECK-NEXT:    %a = fsub float %m, %s
; CHECK-NEXT:    %r = select i1 %c, float %a, float %s
; CHECK-NEXT:    %b = fadd float %s, %m
; CHECK-NEXT:    %q = select i1 %c, float %b, float %s
; CHECK-NEXT:    store float %b, ptr %out
; CHECK-NEXT:    %k = fadd float %s, 2.000000e+00
; CHECK-NEXT:    %p = select i1 %c, float %k, float %s
define float @kept(i1 %c, float %s, float %m, ptr %out) {
  %a = fsub float %m, %s
  %r = select i1 %c, float %a, float %s
  %b = fadd float %s, %m
  %q = select i1 %c, float %b, float %s
  store float %b, ptr %out
  %k = fadd float %s, 2.0
  %p = select i1 %c, float %k, float %s
  %t = fadd float %r, %q
  %u = fadd float %t, %p
  ret float %u
}

; The add's flags, but the select's for what only s decides.
; CHECK-LABEL: @flags(
; CHECK-NEXT:    [[SEL:%.*]] = select i1 %c, float %m, float -0.000000e+00
; CHECK-NEXT:    [[R:%.*]] = fadd contract float %s, [[SEL]]
define float @flags(i1 %c, float %s, float %m) {
  %a = fadd nnan contract float %s, %m
  %r = select i1 %c, float %a, float %s
  ret float %r
}
