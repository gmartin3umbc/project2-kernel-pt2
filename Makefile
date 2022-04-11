obj-y := buffer_sem.o

test_sem: test_sem.c
	gcc -g -Wall -pthread -o test_sem test_sem.c

clean:
	rm test_sem
