all: clean main

main: main.c
	gcc -c main.c list.c
	gcc -o main  main.o list.o -lm
	
clean:
	rm -f main *.o
