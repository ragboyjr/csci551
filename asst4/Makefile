all: bin/gauss-serial

bin/gauss-serial: gauss-serial.o matrix.o vector.o bin
	gcc -o bin/gauss-serial gauss-serial.o matrix.o vector.o -lm
	
bin:
	mkdir bin

gauss-serial.o: gauss-serial.c
	gcc -c gauss-serial.c

matrix.o: matrix.c
	gcc -c matrix.c

matrix.c: matrix.h

vector.o: vector.c
	gcc -c vector.c

vector.c: vector.h

clean:
	rm -rf *.o bin
