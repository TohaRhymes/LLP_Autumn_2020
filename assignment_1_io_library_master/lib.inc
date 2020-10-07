section .data
	buf : times 256 db 0

section .twqytewyiy
 
 
; +Принимает код возврата и завершает текущий процесс
exit: 
    xor rdi, rdi     ; exit code 0
    mov rax, 60      ; system call for exit
    syscall          ; invoke operating system to exit

; +Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax                    ; обнуляем регистр
    .loop:
        cmp byte [rdi+rax], 0       ; сравниваем с нулем очередной символ
        je .end                     ; если равно нулю - выход (смотрим в регистр состояния)
        inc rax                     ; иначе - увеличиваем счетчик на 1
        jmp .loop 
    .end:
        ret

; +Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    call string_length              ; помещаем в rax длину строки
    mov rsi, rdi                    ; source index (откуда - передаваемая ссылка)
    mov rdi, 1                      ; destination index (куда - stdout)
    mov rdx, rax                    ; сколько выводить
    mov rax, 1                      ; sys_write
    syscall
    ret

; +Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov rdi, rsp                    ; source index (откуда - вершина стэка)
    call print_string 
    pop rdi
    ret

; +Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 10
    jmp print_char

; +Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    mov rax, rdi                   ; сохраняем переданное число в "аккумулятор"
    mov rdi, rsp                   ; запоминаем адрес вершины стека
    push 0                         ; делаем "границу"
    sub rsp, 16                    ; выделяем буфер в стэке

    mov r9, 10
    
    dec rdi
    .loop:
        xor rdx, rdx
        div r9                     ; целочисленное деление на 10 с записью в остатка в rdx
        add rdx, '0'               ; остаток + код первой цифры в ASCII
        dec rdi 
        mov [rdi], dl              ; сохраняем 1 байт на стэк
        test rax, rax              ; устанавливает флаги (ZF = 0, если rax = 0)
        jnz .loop                  ; продолжаем, если не ноль
   
    call print_string
    
    add rsp, 24                    ; освобождаем буффер
    ret


; +Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    test rdi, rdi
    jns print_uint
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    neg rdi
    jmp print_uint

; +Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
    mov al, byte [rdi]
    cmp al, byte [rsi]
    jne .not_eq

    inc rdi
    inc rsi

    test al, al
    jnz string_equals
.yes:
    mov rax, 1
    ret
.not_eq:
    xor rax, rax
    ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
    push 0                         ; место для записи символа
    xor rax, rax                   ; код системного вызова sys_read
    xor rdi, rdi                   ; откуда читаем (stdin)
    mov rsi, rsp                   ; куда записываем (метка на стэке)
    mov rdx, 1                     ; сколько читаем
    syscall
    pop rax                        ; сохраняем символ в rax
    ret

; +Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
read_word:
    push rbx                       
    mov r8, rsi                    ; buffer len
    mov r9, rdi                    ; buffer adress
    xor  rbx, rbx                  ;
    xor  rdi, rdi                  ; read from stdin
    mov  rdx, 1                    ; read 1 byte
.strip:
    xor rax, rax                   ; code of sys_read
    mov rsi, buf                   ; adress, where to
    syscall
    cmp al, 0                      ; end of string
    je  .end
    cmp byte[buf], 0x21
    jb  .strip
    inc rbx
.read:
    xor rax, rax                   ; sys_read code
    lea rsi, [buf + rbx]           ; read from this adress
    syscall
    cmp byte [buf + rbx], 0x21
    jb  .end
    cmp r8, rbx                    ; will it fit in size?
    jbe .exit
    inc rbx
    jmp .read
.end:
    mov byte[buf + rbx], 0         ; add end-of-string
    mov rdx, rbx                   ; len()
    mov rax, buf                   ; buffer-pointer
    pop rbx
    ret
.exit:
	xor rdx, r8                    ; buffer len
	xor rax, rax
	pop rbx
	ret

 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    mov r8, 10
    xor rax, rax
    xor rcx, rcx
.loop:
    movzx r9, byte [rdi + rcx] 
    cmp r9b, '0'
    jb .end
    cmp r9b, '9'
    ja .end
    xor rdx, rdx 
    mul r8                         ; rax * 10 + r9b    
    and r9b, 0x0f
    add rax, r9
    inc rcx
    jmp .loop 
.end:
    mov rdx, rcx
    ret




; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    cmp byte[rdi], '-'
    je .negative
    jmp parse_uint
.negative:
    inc rdi                        ; pass minus
    call parse_uint
    test rdx, rdx
    jz .exit                       ; if there is no digits, exit
    inc rdx                        ; count minus
    neg rax
    ret
.exit:
    xor rax, rax
    ret

; +Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    push rdi                       ; указатель на строку
    push rsi                       ; указатель на буфер (куда будем копировать)
    push rdx                       ; длина буфера
    call string_length
    pop rdx
    pop rsi
    pop rdi

    cmp rax, rdx                   ; сравниваем длину строки и длину буфера
    jl .continue                   ; если места хватает, не уходим
    
    xor rax, rax                   ; если не умещается, то выходим и возвращаем 0
    ret
    
    .continue:

    .loop: 
        mov dl, byte[rdi]
        mov byte[rsi], dl
        inc rdi                    ; увеличиваем для следующего символа
        inc rsi
        test dl, dl                ; sets the SF, ZF, and PF status flags according to the result.
        jnz .loop 

    ret





























    
    
