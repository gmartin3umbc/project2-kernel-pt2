#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#include "buffer_sem.h"

#define __NR_init_buffer_sem_421 446
#define __NR_enqueue_buffer_sem_421 447
#define __NR_dequeue_buffer_sem_421 448
#define __NR_delete_buffer_sem_421 449

long test_init_sem(void){
    return syscall(__NR_init_buffer_sem_421);
}

long test_enqueue_sem(char* data){
    return syscall(__NR_enqueue_buffer_sem_421, data);
}

long test_dequeue_sem(char* data){
    return syscall(__NR_dequeue_buffer_sem_421, data);
}

long test_delete_sem(void){
    return syscall(__NR_delete_buffer_sem_421);
}

void *producer()
{
    srand(time(0));

    for (size_t i = 0; i < 1000; i++)
    {
        char data[1024];

        for (size_t j = 0; j < 1024; j++)
        {
            data[j] = (i % 10) + '0';
        }

        // wait between 0 - 10 milliseconds
        int random = (rand() % 11);
        float sleep_time = random * 1000;
        usleep(sleep_time);

        test_enqueue_sem(data);
    }

    return 0;
}

void *consumer()
{
    srand(time(NULL));

    for (size_t i = 0; i < 1000; i++)
    {
        char data[1024];

        // wait between 0 - 10 milliseconds
        int random = (rand() % 11);
        float sleep_time = random * 1000;
        usleep(sleep_time);

        test_dequeue_sem(data);
    }

    return 0;
}

int main()
{

    test_init_sem();

    pthread_t prod, cons;

    if (pthread_create(&prod, NULL, producer, NULL) != 0)
    {
        perror("Failed to create producer thread");
    }

    if (pthread_create(&cons, NULL, consumer, NULL) != 0)
    {
        perror("Failed to create consumer thread");
    }

    if (pthread_join(prod, NULL) != 0)
    {
        perror("Failed to join producer thread");
    }

    if (pthread_join(cons, NULL) != 0)
    {
        perror("Failed to join consumer thread");
    }

    test_delete_sem();

    return 0;
}
