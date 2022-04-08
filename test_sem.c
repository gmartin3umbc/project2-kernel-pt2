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

    for (size_t i = 0; i < 10; i++)
    {
        char data[1024];

        for (size_t j = 0; j < 1024; j++)
        {
            data[j] = (i % 10) + '0';
        }

        test_enqueue_sem(data);

        // print_semaphores();
        // print_semaphores();

        // wait between 0 - 10 milliseconds
        int random = (rand() % 11);
        float sleep_time = random * 1000;
        usleep(sleep_time);

        // sleep(1);
    }

    return 0;
}

void *consumer()
{
    srand(time(NULL));

    for (size_t i = 0; i < 10; i++)
    {
        char data[1024];
        test_dequeue_sem(data);

        // print_semaphores();
        // print_semaphores();

        // wait between 0 - 10 milliseconds
        int random = (rand() % 11);
        float sleep_time = random * 1000;
        usleep(sleep_time);

        // sleep(1);
    }

    return 0;
}

int main()
{

    test_init_sem();

    pthread_t th[2];

    if (pthread_create(&th[1], NULL, &producer, NULL) != 0)
    {
        perror("Failed to create producer thread");
    }

    if (pthread_create(&th[2], NULL, &consumer, NULL) != 0)
    {
        perror("Failed to create consumer thread");
    }

    if (pthread_join(th[1], NULL) != 0)
    {
        perror("Failed to join producer thread");
    }

    if (pthread_join(th[2], NULL) != 0)
    {
        perror("Failed to join consumer thread");
    }

    test_delete_sem();

    return 0;
}
