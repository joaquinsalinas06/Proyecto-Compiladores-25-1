.data
print_fmt: .string "%ld\n"
print_float_fmt: .string "%.6g\n"
print_bool_true: .string "true\n"
print_bool_false: .string "false\n"

.text
.globl mostrar
mostrar:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    movq $42, %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $1, %rax
    jmp .end_mostrar
.end_mostrar:
    leave
    ret
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $256, %rsp
    call mostrar
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt(%rip), %rdi
    movl $0, %eax
    call printf@PLT
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
