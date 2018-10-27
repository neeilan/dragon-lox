target triple = "i386-pc-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%f\00", align 1

define double @double(double %x) {
entry:
  %multmp = fmul double %x, 2.000000e+00
  ret double %multmp
}
  

define i32 @main() {
entry:
  %calltmp = call double @double(double 5.000000e+00)
  %0 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]*  @.str, i32 0, i32 0), double %calltmp)

  ret i32 1
}

declare i32 @printf(i8*, ...)

