.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"
num: .quad 5
factorial: .quad 1

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
.while_start_0:
    movq num(%rip), %rax
    pushq %rax
    movq $0, %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movl $0, %eax
    setg %al
    movzbq %al, %rax
    cmpq $0, %rax
    je .while_end_0
    movq factorial(%rip), %rax
    pushq %rax
    movq num(%rip), %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, factorial(%rip)
    movq num(%rip), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax
    movq %rax, num(%rip)
    jmp .while_start_0
.while_end_0:
    movq factorial(%rip), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
