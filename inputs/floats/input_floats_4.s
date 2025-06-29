.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
a: .double 15.7
b: .double 8.2

.section .rodata
.align 8
.L_float_1:
    .double 8.2
.L_float_0:
    .double 15.7
.L_neg_zero:
    .double -0.0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movsd a(%rip), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd b(%rip), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    mulsd %xmm1, %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd a(%rip), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd b(%rip), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    movsd %xmm1, %xmm2
    divsd %xmm0, %xmm2
    movsd %xmm2, %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd a(%rip), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd b(%rip), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    movsd %xmm1, %xmm2
    subsd %xmm0, %xmm2
    movsd %xmm2, %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
