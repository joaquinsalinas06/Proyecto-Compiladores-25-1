.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
nums_data:
    .quad 1
    .quad 2
    .quad 3
    .quad 4
    .quad 5
nums: .quad nums_data
efe_data:
    .double 1.5
    .double 2.5
    .double 3.5
efe: .quad efe_data

.section .rodata
.align 8
.L_float_2:
    .double 3.5
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
    movq $5, %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    movq %rax, -16(%rbp)
.for_start_0:
    movq -16(%rbp), %rax
    cmpq $5, %rax
    jge .for_end_0
    movq nums(%rip), %rax
    movq %rax, %rbx
    movq -16(%rbp), %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movq (%rbx), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -16(%rbp), %rax
    incq %rax
    movq %rax, -16(%rbp)
    jmp .for_start_0
.for_end_0:
    movq efe(%rip), %rax
    movq %rax, %rbx
    movq $0, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movsd (%rbx), %xmm0
    subq $8, %rsp
    movsd %xmm0, (%rsp)
    movq efe(%rip), %rax
    movq %rax, %rbx
    movq $1, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movsd (%rbx), %xmm0
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
