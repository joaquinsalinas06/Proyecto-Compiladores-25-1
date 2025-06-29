.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
price: .double 99.99
tax: .double 8.5
total: .double 0

.section .rodata
.align 8
.L_float_2:
    .double 0
.L_float_1:
    .double 8.5
.L_float_0:
    .double 99.99
.L_neg_zero:
    .double -0.0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movsd price(%rip), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movsd tax(%rip), %xmm0
    movsd (%rsp), %xmm1
    addq $8, %rsp
    addsd %xmm1, %xmm0
    movsd %xmm0, total(%rip)
    movsd price(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd tax(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
    movsd total(%rip), %xmm0
    leaq print_float_fmt(%rip), %rdi
    movl $1, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
