global _start

section .bss
RAM resw 500
BUFF resb 100

section .text

_start:
mov r15, RAM
push QWORD 5
pop QWORD [r15 + 0]
push QWORD 1
pop QWORD [r15 + 8]
.while_start0:
push QWORD [r15 + 0]
push QWORD 1
pop QWORD r11
pop QWORD r10
cmp r10, r11
jle .while_stop0
push QWORD [r15 + 8]
push QWORD [r15 + 0]
pop QWORD r11
pop QWORD r10
imul r10, r11
push QWORD r10
pop QWORD [r15 + 8]
push QWORD [r15 + 0]
push QWORD 1
pop QWORD r11
pop QWORD r10
sub r10, r11
push QWORD r10
pop QWORD [r15 + 0]
jmp .while_start0
.while_stop0:
push QWORD [r15 + 8]
pop QWORD [r15 + 16]
add r15, 16
call my_print
sub r15, 16
mov rax, 60
syscall

my_input:
    push r10
    push r11
    push r15

    mov rax, 0x0
    mov rdi, 1
    mov rsi, BUFF
    mov rdx, 100
    syscall

    mov rcx, rax
    mov rsi, BUFF
    xor rbx, rbx
    mov r15, 10

    xor r11, r11
    mov al, byte [rsi]
    cmp al, '-'
    jne .loop
    mov r11, 1
    inc rsi

    .loop:
        xor rax, rax
        mov al, byte [rsi]
        sub al, '0'
        imul rax, r15
        add rbx, rax

        dec rcx
        cmp rcx, 0
        jne .loop

    cmp r11, 1
    jne .next
    neg rbx

    .next:

    pop r15
    pop r11
    pop r10

    pop rax
    push rbx
    push rax

    ret

my_print:
    push r10
    push r11
    push r15

    mov rax, [r15]
    mov rsi, BUFF
    mov rcx, 99
    mov r15, 10
    mov [BUFF + 100], r15

    mov r12b, '0'
    cmp rax, 0
    jge .loop
    neg rax
    mov r12b, '-'

    .loop:
        xor rdx, rdx    ; rdx = 0
        cqo
        idiv r15        ; rax /= 10, rdx = rax % 10

        add rdx, '0'
        mov byte [BUFF + rcx], dl
        dec rcx

        cmp rax, 0
        je .loop

    mov [BUFF + rcx], r12b
    add rsi, rcx
    mov rax, 0x01
    mov rdi, 1
    mov rdx, 100
    sub rdx, rcx

    cmp r12b, '0'
    je .next
    dec rsi
    inc rdx

    .next:
    syscall

    pop r15
    pop r11
    pop r10

    ret