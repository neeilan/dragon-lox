; ModuleID = 'Default module'
source_filename = "Default module"

define double @double(double %x) {
entry:
  %multmp = fmul double %x, 2.000000e+00
  ret double %multmp
}

define double @main() {
entry:
  %calltmp = call double @double(double 5.000000e+00)
}
