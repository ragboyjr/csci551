
all: bin bin/matrixmult-serial bin/matrixmult

bin:
	mkdir bin

bin/matrixmult: matrixmult.c
	mpicc -g -Wall -std=c99 matrixmult.c -o bin/matrixmult

bin/matrixmult-serial: matrixmult-serial.c
	clang matrixmult-serial.c -o bin/matrixmult-serial -O3
	
clean:
	rm -rf bin
