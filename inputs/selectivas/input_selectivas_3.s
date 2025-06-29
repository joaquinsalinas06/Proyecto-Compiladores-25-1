.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
counter: .quad 0
sum: .quad 0

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
.while_start_0:
    movq counter(%rip), %rax
    pushq %rax
    movq $4, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je .while_end_0
    movq sum(%rip), %rax
    pushq %rax
    movq counter(%rip), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, sum(%rip)
    movq counter(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq sum(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq counter(%rip), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, counter(%rip)
    jmp .while_start_0
.while_end_0:
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
