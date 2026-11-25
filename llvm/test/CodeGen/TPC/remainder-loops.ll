; RUN: opt -mtriple=tpc -mcpu=gaudi2 -passes='loop(tpc-remainder-loops)' -S %s | FileCheck %s
; RUN: opt -mtriple=tpc -mcpu=gaudi2 -passes='loop(tpc-remainder-loops)' -tpc-remainder-loop-trips=0 -S %s | FileCheck %s --check-prefix=OFF

; for (; i + 512 <= n; i += 512) ...; for (; i < n; i += 64) ...: the second
; loop starts at 0 when n < 512 and where the first left off otherwise, with
; i + 512 > n either way, so it runs at most 8 times and is kept from the
; runtime unroller. The first loop, and a loop from a start nothing bounds,
; are not.

define float @two(i32 %n) {
entry:
  %small = icmp slt i32 %n, 512
  br i1 %small, label %rest.pre, label %big

big:
  %i = phi i32 [ 0, %entry ], [ %i.next, %big ]
  %s = phi float [ 0.0, %entry ], [ %s.next, %big ]
  %f = sitofp i32 %i to float
  %s.next = fadd float %s, %f
  %i.next = add nuw nsw i32 %i, 512
  %end = add nuw nsw i32 %i.next, 512
  %done = icmp sgt i32 %end, %n
  br i1 %done, label %big.exit, label %big, !llvm.loop !0

big.exit:
  %i.lcssa = phi i32 [ %i.next, %big ]
  %s.lcssa = phi float [ %s.next, %big ]
  br label %rest.pre

rest.pre:
  %start = phi i32 [ 0, %entry ], [ %i.lcssa, %big.exit ]
  %s.start = phi float [ 0.0, %entry ], [ %s.lcssa, %big.exit ]
  %any = icmp slt i32 %start, %n
  br i1 %any, label %rest, label %out

rest:
  %j = phi i32 [ %start, %rest.pre ], [ %j.next, %rest ]
  %t = phi float [ %s.start, %rest.pre ], [ %t.next, %rest ]
  %g = sitofp i32 %j to float
  %t.next = fadd float %t, %g
  %j.next = add nuw nsw i32 %j, 64
  %more = icmp slt i32 %j.next, %n
  br i1 %more, label %rest, label %out, !llvm.loop !2

out:
  %r = phi float [ %s.start, %rest.pre ], [ %t.next, %rest ]
  ret float %r
}

define float @unbounded(i32 %start, i32 %n) {
entry:
  %any = icmp slt i32 %start, %n
  br i1 %any, label %loop, label %out

loop:
  %j = phi i32 [ %start, %entry ], [ %j.next, %loop ]
  %t = phi float [ 0.0, %entry ], [ %t.next, %loop ]
  %g = sitofp i32 %j to float
  %t.next = fadd float %t, %g
  %j.next = add nuw nsw i32 %j, 64
  %more = icmp slt i32 %j.next, %n
  br i1 %more, label %loop, label %out, !llvm.loop !4

out:
  %r = phi float [ 0.0, %entry ], [ %t.next, %loop ]
  ret float %r
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.mustprogress"}
!2 = distinct !{!2, !1}
!4 = distinct !{!4, !1}

; CHECK-LABEL: define float @two(
; CHECK:       br i1 %done, label %big.exit, label %big, !llvm.loop [[BIG:![0-9]+]]
; CHECK:       br i1 %more, label %rest, label %out{{.*}}, !llvm.loop [[REST:![0-9]+]]
; CHECK-LABEL: define float @unbounded(
; CHECK:       br i1 %more, label %loop, label %out{{.*}}, !llvm.loop [[UNB:![0-9]+]]
; CHECK-DAG:   [[BIG]] = distinct !{[[BIG]], [[MP:![0-9]+]]}
; CHECK-DAG:   [[REST]] = distinct !{[[REST]], [[MP]], [[RT:![0-9]+]]}
; CHECK-DAG:   [[RT]] = !{!"llvm.loop.unroll.runtime.disable"}
; CHECK-DAG:   [[UNB]] = distinct !{[[UNB]], [[MP]]}

; OFF-NOT: llvm.loop.unroll.runtime.disable
