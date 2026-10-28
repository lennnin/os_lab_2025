CC=gcc
CFLAGS=-I.

all: parallel_min_max run_parallel process_memory parallel_sum

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h parallel_min_max.c
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

run_parallel : run_parallel.c
	$(CC) -o run_parallel run_parallel.c $(CFLAGS)

utils.o : utils.h utils.c
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h find_min_max.c
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

parallel_sum : parallel_sum.c utils.o utils.h
	$(CC) -o parallel_sum parallel_sum.c utils.o $(CFLAGS) -pthread

process_memory : process_memory.c
	$(CC) -o process_memory process_memory.c -w $(CFLAGS)

clean :
	rm -f utils.o find_min_max.o parallel_min_max run_parallel parallel_sum process_memory /tmp/minmax_*.txt