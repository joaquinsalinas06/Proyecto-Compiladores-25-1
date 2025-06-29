.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
temperature: .double 25.5
increment: .double 2.8

.section .rodata
.align 8
.L_float_2:
    .double 5
.L_float_1:
    .double 2.8
.L_float_0:
    .double 25.5
.L_neg_zero:
    .double -0.0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movsd temperature(%rip), %xmm0
    movsd %xmm0, %xmm1
    movsd increment(%rip), %xmm0
    addsd %xmm1, %xmm0
    movsd %xmm0, temperature(%rip)
    movsd temperature(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd temperature(%rip), %xmm0
    movsd %xmm0, %xmm1
    movsd .L_float_2(%rip), %xmm0
    movsd %xmm1, %xmm2
    subsd %xmm0, %xmm2
    movsd %xmm2, %xmm0
    movsd %xmm0, temperature(%rip)
    movsd temperature(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
