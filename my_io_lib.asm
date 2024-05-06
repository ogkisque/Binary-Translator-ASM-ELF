nop
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
    dec rcx
    xor rax, rax
    xor rbx, rbx
    mov r15, 10

    xor r11, r11
    mov al, byte [rsi]
    cmp al, '-'
    jne .loop
    mov r11, 1
    inc rsi
    dec rcx

    .loop:
        xor r13, r13
        mov r13b, byte [rsi]
        sub r13b, '0'
        mov rax, rbx
        mul r15
        add rax, r13
        mov rbx, rax

        inc rsi
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
    mov [rsi + 100], r15b

    mov r12b, '0'
    cmp rax, 0
    jge .loop
    neg rax
    mov r12b, '-'

    .loop:
        xor rdx, rdx    ; rdx = 0
        cqo
        div r15         ; rax /= 10, rdx = rax % 10

        add rdx, '0'
        mov byte [rsi + rcx], dl
        dec rcx

        cmp rax, 0
        jne .loop

    mov [rsi + rcx], r12b
    add rsi, rcx
    add rsi, 1
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
nop