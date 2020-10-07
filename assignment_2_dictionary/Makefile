ASFLAGS=-f elf64
ASM=nasm
LD = ld

# пересобрать = удалить старые + создать новые
default: clean main

# собрать все
main: main.o dict.o lib.o 
	$(LD) -o main main.o dict.o lib.o

main.o: main.asm colon.inc words.inc
	$(ASM) $(ASFLAGS) -o main.o main.asm

lib.o: lib.inc
	$(ASM) $(ASFLAGS) -o lib.o lib.inc

dict.o: dict.asm 
	$(ASM) $(ASFLAGS) -o dict.o dict.asm

# удалить старые версии
clean:
	rm -f main.o lib.o dict.o main
