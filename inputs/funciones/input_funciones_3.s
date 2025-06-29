.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"

.text
.globl factorial
factorial:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    subq $256, %rsp
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setle %al
    movzbq %al, %rax
    cmpq $0, %rax
    je .else_0
    movq $1, %rax
    jmp .end_factorial
    jmp .endif_0
.else_0:
    movq -8(%rbp), %rax
    pushq %rax
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rdi
    call factorial
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    jmp .end_factorial
.endif_0:
.end_factorial:
    leave
    ret
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq $5, %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rdi
    call factorial
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -24(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
