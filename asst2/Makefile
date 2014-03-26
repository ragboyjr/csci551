
all: find-n integrate 

bin:
	mkdir bin

# Find n

find-n: find-n.o bin asst.o
	clang find-n.o asst.o -o bin/find-n -lm

find-n.o: find-n.c asst.h
	clang -c find-n.c

# Assignment
asst.o: asst.c asst.h
	clang -c asst.c

# Integrate

integrate: integrate.o asst.o bin
	mpicc integrate.o asst.o -lm -o bin/integrate

integrate.o: integrate.c
	mpicc -g -Wall -std=c99 -c integrate.c

clean:
	rm bin/* *.o
