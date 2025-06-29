.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"

.text
.globl calcular
calcular:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    subq $256, %rsp
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    pushq %rax
    movq $10, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -32(%rbp)
    movq -32(%rbp), %rax
    jmp .end_calcular
.end_calcular:
    leave
    ret
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq $5, %rax
    movq %rax, -16(%rbp)
    movq $3, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rdi
    movq -24(%rbp), %rax
    movq %rax, %rsi
    call calcular
    movq %rax, -32(%rbp)
    movq -32(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
