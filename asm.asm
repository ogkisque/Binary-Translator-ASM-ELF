global _start

section .text

%INCLUDE "my_io_lib.asm"

func0:
push QWORD [r15 + 0]
push QWORD 1
pop QWORD r11
pop QWORD r10
cmp r10, r11
jne .if0
push QWORD [r15 + 0]
pop QWORD r10
pop QWORD r11
push QWORD r10
push QWORD r11
ret
.if0:
push QWORD [r15 + 0]
push QWORD 1
pop QWORD r11
pop QWORD r10
cmp r10, r11
jle .if1
push QWORD [r15 + 0]
push QWORD [r15 + 0]
push QWORD 1
pop QWORD r11
pop QWORD r10
sub r10, r11
push QWORD r10
pop QWORD [r15 + 16]
add r15, 16
call func0
sub r15, 16
pop QWORD r11
pop QWORD r10
imul r10, r11
push QWORD r10
pop QWORD [r15 + 8]
push QWORD [r15 + 8]
pop QWORD r10
pop QWORD r11
push QWORD r10
push QWORD r11
ret
.if1:

_start:
mov r15, RAM
push QWORD 8
pop QWORD [r15 + 0]
push QWORD 10000
pop QWORD [r15 + 8]
.while_start0:
push QWORD [r15 + 8]
push QWORD 0
pop QWORD r11
pop QWORD r10
cmp r10, r11
jle .while_stop0
push QWORD [r15 + 0]
pop QWORD [r15 + 24]
add r15, 24
call func0
sub r15, 24
pop QWORD [r15 + 16]
push QWORD [r15 + 8]
push QWORD 1
pop QWORD r11
pop QWORD r10
sub r10, r11
push QWORD r10
pop QWORD [r15 + 8]
jmp .while_start0
.while_stop0:
push QWORD [r15 + 16]
pop QWORD [r15 + 24]
add r15, 24
call my_print
sub r15, 24
mov rax, 60
syscall

section .data
RAM: dq 500 dup 0
BUFF: db 100 dup 0
