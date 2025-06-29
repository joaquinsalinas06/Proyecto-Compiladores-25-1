.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
radius: .double 5
pi: .double 3.14
area: .double 0

.section .rodata
.align 8
.L_float_2:
    .double 0
.L_float_1:
    .double 3.14
.L_float_0:
    .double 5
.L_neg_zero:
    .double -0.0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movsd pi(%rip), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd radius(%rip), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    mulsd %xmm1, %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd radius(%rip), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    mulsd %xmm1, %xmm0
    movsd %xmm0, area(%rip)
    movsd radius(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd pi(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd area(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
