; ModuleID = 'microcodes.c'
source_filename = "microcodes.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.state = type { [2048 x i8], i16, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i16, i32, i64, i8, i8, i8, i8 }

@cpuState = common global %struct.state zeroinitializer, align 8

; Function Attrs: norecurse nounwind
define void @read(%struct.state* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 13
  %3 = load i16, i16* %2, align 2, !tbaa !2
  %4 = icmp ult i16 %3, 8192
  br i1 %4, label %5, label %11

; <label>:5:                                      ; preds = %1
  %6 = and i16 %3, 2047
  %7 = zext i16 %6 to i64
  %8 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 0, i64 %7
  %9 = load i8, i8* %8, align 1, !tbaa !10
  %10 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 12
  store i8 %9, i8* %10, align 4, !tbaa !11
  br label %11

; <label>:11:                                     ; preds = %5, %1
  ret void
}

; Function Attrs: norecurse nounwind
define void @readPc(%struct.state* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 1
  %3 = load i16, i16* %2, align 8, !tbaa !12
  %4 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 13
  store i16 %3, i16* %4, align 2, !tbaa !2
  tail call void @read(%struct.state* %0)
  ret void
}

; Function Attrs: norecurse nounwind
define void @readPcAndInc(%struct.state* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 1
  %3 = load i16, i16* %2, align 8, !tbaa !12
  %4 = add i16 %3, 1
  store i16 %4, i16* %2, align 8, !tbaa !12
  %5 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 13
  store i16 %3, i16* %5, align 2, !tbaa !2
  tail call void @read(%struct.state* %0)
  ret void
}

; Function Attrs: norecurse nounwind
define void @testInterrupt(%struct.state* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 16
  %3 = load i8, i8* %2, align 8, !tbaa !13, !range !14
  %4 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 17
  %5 = load i8, i8* %4, align 1, !tbaa !15, !range !14
  %6 = or i8 %5, %3
  %7 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 9
  %8 = load i8, i8* %7, align 1, !tbaa !16, !range !14
  %9 = xor i8 %8, -1
  %10 = and i8 %6, %9
  %11 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 18
  %12 = load i8, i8* %11, align 2, !tbaa !17, !range !14
  %13 = or i8 %10, %12
  %14 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 19
  store i8 %13, i8* %14, align 1, !tbaa !18
  ret void
}

; Function Attrs: norecurse nounwind
define void @addClocks(%struct.state* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 14
  %3 = load i32, i32* %2, align 8, !tbaa !19
  %4 = add nsw i32 %3, 3
  store i32 %4, i32* %2, align 8, !tbaa !19
  ret void
}

; Function Attrs: norecurse nounwind
define void @SetFlagI(%struct.state* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 9
  store i8 1, i8* %2, align 1, !tbaa !16
  ret void
}

; Function Attrs: norecurse nounwind
define void @ClearFlagD(%struct.state* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.state, %struct.state* %0, i64 0, i32 8
  store i8 0, i8* %2, align 8, !tbaa !20
  ret void
}

; Function Attrs: norecurse nounwind
define i32 @main() local_unnamed_addr #0 {
  store i16 65, i16* getelementptr inbounds (%struct.state, %struct.state* @cpuState, i64 0, i32 1), align 8, !tbaa !12
  store i8 0, i8* getelementptr inbounds (%struct.state, %struct.state* @cpuState, i64 0, i32 2), align 2, !tbaa !21
  store i8 0, i8* getelementptr inbounds (%struct.state, %struct.state* @cpuState, i64 0, i32 3), align 1, !tbaa !22
  store i8 -1, i8* getelementptr inbounds (%struct.state, %struct.state* @cpuState, i64 0, i32 4), align 4, !tbaa !23
  store i8 -3, i8* getelementptr inbounds (%struct.state, %struct.state* @cpuState, i64 0, i32 5), align 1, !tbaa !24
  store i8 1, i8* getelementptr inbounds (%struct.state, %struct.state* @cpuState, i64 0, i32 9), align 1, !tbaa !16
  tail call void @addClocks(%struct.state* nonnull @cpuState)
  tail call void @readPc(%struct.state* nonnull @cpuState)
  tail call void @read(%struct.state* nonnull @cpuState)
  tail call void @readPcAndInc(%struct.state* nonnull @cpuState)
  tail call void @testInterrupt(%struct.state* nonnull @cpuState)
  tail call void @SetFlagI(%struct.state* nonnull @cpuState)
  tail call void @ClearFlagD(%struct.state* nonnull @cpuState)
  %1 = load i8, i8* getelementptr inbounds (%struct.state, %struct.state* @cpuState, i64 0, i32 12), align 4, !tbaa !11
  %2 = zext i8 %1 to i32
  ret i32 %2
}

attributes #0 = { norecurse nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
!2 = !{!3, !6, i64 2062}
!3 = !{!"state", !4, i64 0, !6, i64 2048, !4, i64 2050, !4, i64 2051, !4, i64 2052, !4, i64 2053, !7, i64 2054, !7, i64 2055, !7, i64 2056, !7, i64 2057, !7, i64 2058, !7, i64 2059, !4, i64 2060, !6, i64 2062, !8, i64 2064, !9, i64 2072, !7, i64 2080, !7, i64 2081, !7, i64 2082, !7, i64 2083}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!"short", !4, i64 0}
!7 = !{!"_Bool", !4, i64 0}
!8 = !{!"int", !4, i64 0}
!9 = !{!"long", !4, i64 0}
!10 = !{!4, !4, i64 0}
!11 = !{!3, !4, i64 2060}
!12 = !{!3, !6, i64 2048}
!13 = !{!3, !7, i64 2080}
!14 = !{i8 0, i8 2}
!15 = !{!3, !7, i64 2081}
!16 = !{!3, !7, i64 2057}
!17 = !{!3, !7, i64 2082}
!18 = !{!3, !7, i64 2083}
!19 = !{!3, !8, i64 2064}
!20 = !{!3, !7, i64 2056}
!21 = !{!3, !4, i64 2050}
!22 = !{!3, !4, i64 2051}
!23 = !{!3, !4, i64 2052}
!24 = !{!3, !4, i64 2053}
