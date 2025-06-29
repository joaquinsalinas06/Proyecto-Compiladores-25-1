.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
age: .quad 20
status: .quad 0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq age(%rip), %rax
    pushq %rax
    movq $18, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setge %al
    movzbq %al, %rax
    cmpq $0, %rax
    je .else_0
    movq age(%rip), %rax
    pushq %rax
    movq $65, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setge %al
    movzbq %al, %rax
    cmpq $0, %rax
    je .else_1
    movq $3, %rax
    movq %rax, status(%rip)
    movq status(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    jmp .endif_1
.else_1:
    movq $2, %rax
    movq %rax, status(%rip)
    movq status(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.endif_1:
    jmp .endif_0
.else_0:
    movq $1, %rax
    movq %rax, status(%rip)
    movq status(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.endif_0:
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
