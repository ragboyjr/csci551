all: bin/gauss-eliminate

bin/gauss-eliminate: gauss-eliminate.o matrix.o vector.o bin
	gcc -o bin/gauss-eliminate -lm -fopenmp gauss-eliminate.o matrix.o vector.o
	
bin:
	mkdir bin

gauss-eliminate.o: gauss-eliminate.c
	gcc -c gauss-eliminate.c -fopenmp

matrix.o: matrix.c
	gcc -c matrix.c

matrix.c: matrix.h

vector.o: vector.c
	gcc -c vector.c

vector.c: vector.h

clean:
	rm -rf *.o bin
