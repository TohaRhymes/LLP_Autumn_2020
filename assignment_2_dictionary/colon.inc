%define lw 0

%macro colon 2
    %2: dq lw
    db %1, 0
    %define lw %2
%endmacro



