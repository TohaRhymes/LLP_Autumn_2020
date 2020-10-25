nasm -g "$1".asm -felf64 -o "$1".o
ld -o "$1" "$1".o
./"$1"
