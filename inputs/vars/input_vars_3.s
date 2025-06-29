.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
counter: .quad 0
total: .quad 100

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq counter(%rip), %rax
    pushq %rax
    movq $10, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, counter(%rip)
    movq total(%rip), %rax
    pushq %rax
    movq $25, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, total(%rip)
    movq counter(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq total(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq counter(%rip), %rax
    pushq %rax
    movq total(%rip), %rax
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
