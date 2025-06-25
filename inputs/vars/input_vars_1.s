    movq $10, %rax
    movq %rax, x(%rip)
    movq $20, %rax
    movq %rax, y(%rip)
.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.1f\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
y: .quad 0
x: .quad 0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    movq x(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq y(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq x(%rip), %rax
    pushq %rax
    movq y(%rip), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
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
