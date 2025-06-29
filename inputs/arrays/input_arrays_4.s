.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq $1, %rax
    movq %rax, -56(%rbp)
    movq $2, %rax
    movq %rax, -48(%rbp)
    movq $3, %rax
    movq %rax, -40(%rbp)
    movq $4, %rax
    movq %rax, -32(%rbp)
    movq $5, %rax
    movq %rax, -24(%rbp)
    leaq -56(%rbp), %rax
    movq %rax, -16(%rbp)
    movq $10, %rax
    movq %rax, -88(%rbp)
    movq $20, %rax
    movq %rax, -80(%rbp)
    movq $30, %rax
    movq %rax, -72(%rbp)
    leaq -88(%rbp), %rax
    movq %rax, -64(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $0, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movq (%rbx), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $10, %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $0, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    popq %rax
    movq %rax, (%rbx)
    movq -16(%rbp), %rax
    movq %rax, %rbx
    movq $0, %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movq (%rbx), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    movq %rax, -96(%rbp)
.for_start_0:
    movq -96(%rbp), %rax
    cmpq $3, %rax
    jge .for_end_0
    movq -64(%rbp), %rax
    movq %rax, %rbx
    movq -96(%rbp), %rax
    movq %rax, %rcx
    imulq $8, %rcx
    addq %rcx, %rbx
    movq (%rbx), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -96(%rbp), %rax
    incq %rax
    movq %rax, -96(%rbp)
    jmp .for_start_0
.for_end_0:
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
