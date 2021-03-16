all: speed_test

CC=gcc
CFLAGS= -g -pthread
speed_test: speed_test.o
	$(CC) $(CFLAGS) -o speed_test speed_test.o
speed_test.o:speed_test.c
	$(CC) $(CFLAGS) -c speed_test.c
	
clean:
	rm -f speed_test.o
