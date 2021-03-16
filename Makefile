all: speed_test

CC=gcc
CFLAGS= -std=c99
speed_test: speed_test.o
	gcc -o speed_test speed_test.o
speed_test.o:speed_test.c
	gcc -c speed_test.c
	
clean:
	rm -f speed_test.o
