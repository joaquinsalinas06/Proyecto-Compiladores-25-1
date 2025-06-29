.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
x: .quad 7
y: .quad 2
z: .quad 3

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
    addq %rcx, %rax
    pushq %rax
    movq z(%rip), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq x(%rip), %rax
    pushq %rax
    movq y(%rip), %rax
    pushq %rax
    movq z(%rip), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq x(%rip), %rax
    pushq %rax
    movq y(%rip), %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    pushq %rax
    movq z(%rip), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq x(%rip), %rax
    pushq %rax
    movq y(%rip), %rax
    movq %rax, %rcx
    popq %rax
    cqto
    idivq %rcx
    pushq %rax
    movq z(%rip), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
