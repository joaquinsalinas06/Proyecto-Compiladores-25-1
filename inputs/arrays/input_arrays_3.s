.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"

.section .rodata
.align 8
.L_float_2:
    .double 3
.L_float_1:
    .double 2.5
.L_float_0:
    .double 1.5
.L_neg_zero:
    .double -0.0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movsd .L_float_0(%rip), %xmm0
    movsd %xmm0, -40(%rbp)
    movsd .L_float_1(%rip), %xmm0
    movsd %xmm0, -32(%rbp)
    movsd .L_float_2(%rip), %xmm0
    movsd %xmm0, -24(%rbp)
    leaq -40(%rbp), %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $1, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movsd (%rbx), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $0, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movsd (%rbx), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $1, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movsd (%rbx), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    addsd %xmm1, %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $2, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movsd (%rsp), %xmm0
    addq $8, %rsp
    movsd %xmm0, (%rbx)
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $2, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movsd (%rbx), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
