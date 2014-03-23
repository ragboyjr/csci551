
all: main.o bin
	clang main.o -o bin/main -lm
	
bin:
	mkdir bin

main.o: main.c
	clang -c main.c -O3

clean:
	rm bin/*
