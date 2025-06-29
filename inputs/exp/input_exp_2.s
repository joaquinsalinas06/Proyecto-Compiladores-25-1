.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
a: .quad 10
b: .quad 4

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq a(%rip), %rax
    pushq %rax
    movq b(%rip), %rax
    movq %rax, %rcx
    popq %rax
    cqto
    idivq %rcx
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq a(%rip), %rax
    pushq %rax
    movq b(%rip), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    cqto
    idivq %rcx
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq a(%rip), %rax
    pushq %rax
    movq b(%rip), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    pushq %rax
    movq a(%rip), %rax
    pushq %rax
    movq b(%rip), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rcx
    popq %rax
    cqto
    idivq %rcx
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
