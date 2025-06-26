.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.1f\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    movq $10, %rax
    movq %rax, -8(%rbp)
    movq $5, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -24(%rbp)
    subq $24, %rsp
    movq -24(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
