.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
squares: .quad 0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq $2, %rax
    movq %rax, -16(%rbp)
    movq $4, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax
    movq %rax, -32(%rbp)
.for_start_0:
    movq -32(%rbp), %rax
    movq -24(%rbp), %rcx
    cmpq %rcx, %rax
    jg .for_end_0
    movq -32(%rbp), %rax
    pushq %rax
    movq -32(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, squares(%rip)
    movq -32(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq squares(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -32(%rbp), %rax
    incq %rax
    movq %rax, -32(%rbp)
    jmp .for_start_0
.for_end_0:
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
