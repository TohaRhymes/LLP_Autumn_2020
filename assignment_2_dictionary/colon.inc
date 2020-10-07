%define lw 0

%macro colon 2 ; принимаем два аргумента
%%lw: dq lw 
db %1, 0
xt_ %+ %2:

%define lw %%lw
%endmacro

