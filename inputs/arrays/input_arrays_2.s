.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq $5, %rax
    movq %rax, -16(%rbp)
    movq $1, %rax
    movq %rax, -64(%rbp)
    movq $2, %rax
    movq %rax, -56(%rbp)
    movq $3, %rax
    movq %rax, -48(%rbp)
    movq $4, %rax
    movq %rax, -40(%rbp)
    movq $5, %rax
    movq %rax, -32(%rbp)
    leaq -64(%rbp), %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -24(%rbp), %rax
    movq %rax, %rbx
    movq $0, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movq (%rbx), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -24(%rbp), %rax
    movq %rax, %rbx
    movq $4, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movq (%rbx), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $5, %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
