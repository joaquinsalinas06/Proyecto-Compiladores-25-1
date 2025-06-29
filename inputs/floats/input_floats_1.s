.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
x: .double 10.5
y: .double 20.3

.section .rodata
.align 8
.L_float_1:
    .double 20.3
.L_float_0:
    .double 10.5
.L_neg_zero:
    .double -0.0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movsd x(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd y(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd x(%rip), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd y(%rip), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    addsd %xmm1, %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
