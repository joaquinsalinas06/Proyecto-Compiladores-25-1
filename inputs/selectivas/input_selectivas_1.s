.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
x: .quad 15
y: .quad 10
result: .quad 0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq x(%rip), %rax
    pushq %rax
    movq y(%rip), %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setg %al
    movzbq %al, %rax
    cmpq $0, %rax
    je .else_0
    movq x(%rip), %rax
    pushq %rax
    movq y(%rip), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, result(%rip)
    movq result(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    jmp .endif_0
.else_0:
    movq x(%rip), %rax
    pushq %rax
    movq y(%rip), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, result(%rip)
    movq result(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.endif_0:
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
