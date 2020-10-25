global find_word
extern string_equals


section .text
; rdi = адрес слова
; rsi = адрес последнего слова
; returns: 
;   rax = 0 если не нашли, 
;   иначе rax = address
find_word: 
    xor rax, rax
.loop:
    test rsi, rsi ; проверяем, есть ли что читать
    jz .end 
    push rdi
    push rsi
    add rsi, 8
    call string_equals ; сравниваем наш ключ, и те, которые имеются
    pop rsi
    pop rdi
    test rax, rax
    jnz .found
    mov rsi, [rsi]
    jmp .loop
.found:
    mov rax, rsi
.end:
    ret
