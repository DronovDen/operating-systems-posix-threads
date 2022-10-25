#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>

#define STRINGS_TO_PRINT 10

sem_t sem1;
sem_t sem2;

void *threadRoutine(void *args)
{
    for (int i = 1; i <= STRINGS_TO_PRINT; i++)
    {
        sem_wait(&sem2);
        printf("CHILD string #%d\n", i);
        sleep(2);
        sem_post(&sem1);
    }
    return NULL;
}

int main()
{
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 0);

    pthread_t childThread;

    int error_code = pthread_create(&childThread, NULL, threadRoutine, NULL);

    if (0 != error_code)
    {
        printf("Failed to create thread: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= STRINGS_TO_PRINT; i++)
    {
        sem_wait(&sem1);
        printf("PARENT string #%d\n", i);
        sem_post(&sem2);
    }

    sem_destroy(&sem1);
    sem_destroy(&sem2);

    pthread_exit(0);
}