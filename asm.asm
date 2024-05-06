global _start

section .text

%INCLUDE "my_io_lib.asm"

_start:
mov r15, RAM
call my_input
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

section .data
RAM: dq 500 dup 0
BUFF: db 100 dup 0
