clang -emit-llvm -S microcodes.c -O1 -fno-unwind-tables -fno-exceptions
llc microcodes.ll -march=x86-64 --x86-asm-syntax=intel