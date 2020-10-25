section .data
	buf : times 256 db 0
    str: db 10, 0

section .text

global exit
global string_length
global print_string
global print_char
global print_newline
global print_uint
global print_int
global string_equals
global read_char
global read_word
global parse_uint
global parse_int
global string_copy
global print_error
global print_new_line
 
; ++ Принимает код возврата и завершает текущий процесс
exit: 
    xor rdi, rdi     ; exit code 0
    mov rax, 60      ; system call for exit
    syscall          ; invoke operating system to exit

; ++ Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax                    ; обнуляем регистр
    .loop:
        cmp byte [rdi+rax], 0       ; сравниваем с нулем очередной символ
        je .end                     ; если равно нулю - выход (смотрим в регистр состояния)
        inc rax                     ; иначе - увеличиваем счетчик на 1
        jmp .loop 
    .end:
        ret

; ++ Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length              ; помещаем в rax длину строки
    pop rsi                         ; source index (откуда - передаваемая ссылка)
    mov rdi, 1                      ; destination index (куда - stdout)
    mov rdx, rax                    ; сколько выводить
    mov rax, 1                      ; sys_write
    syscall
    ret

; ++ Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov rdi, rsp                    ; source index (откуда - вершина стэка)
    call print_string 
    pop rdi
    ret

; ++ Переводит строку (выводит символ с кодом 0xA)
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


; ++ Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    test rdi, rdi
    jns print_uint
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    neg rdi
    jmp print_uint

; ++ Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
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

; ++ Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
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
    push r14
    xor r14, r14 

    .A:
    push rdi
    call read_char
    pop rdi

    cmp al, ' '
    je .A
    cmp al, 10
    je .A
    cmp al, 13
    je .A 
    cmp al, 9 
    je .A

    .B:
    mov byte [rdi + r14], al
    inc r14

    push rdi
    call read_char
    pop rdi
    cmp al, ' '
    je .C
    cmp al, 10
    je .C
    cmp al, 13
    je .C 
    cmp al, 9
    je .C
    test al, al
    jz .C
    cmp r14, 254
    je .C 

    jmp .B

    .C:
    mov byte [rdi + r14], 0
    mov rax, rdi 
    
    pop r14
    ret

 

; ++ Принимает указатель на строку, пытается
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


; ++ 
print_error:
    push rdi
    call string_length
    pop rsi
    mov rdx, rax 
    mov rax, 1
    mov rdi, 2 
    syscall
    ret


print_new_line:
    mov rdi, str
    call print_string
    ret
























    
    
