all: clean main

main: main.c
	gcc -c main.c list.c
	gcc -o main  -pedantic -ansi -Waddress -Wfatal-errors -Wnonnull main.o list.o -lm
	
clean:
	rm -f main *.o
