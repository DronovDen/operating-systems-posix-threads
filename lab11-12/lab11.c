#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define COMMON 0
#define PARENT 1
#define CHILD 2
#define SECONDS_TO_SLEEP 1
#define STRINGS_TO_PRINT 5
#define MUTEXES_NUMBER 3

pthread_t thread;
pthread_mutex_t mutexes[MUTEXES_NUMBER];

void destroyMutexes()
{
    for (int i = 0; i < MUTEXES_NUMBER; i++)
    {
        pthread_mutex_destroy(&mutexes[i]);
    }
}

void lockMutex(int mutex_num)
{
    int error_code = pthread_mutex_lock(&mutexes[mutex_num]);
    if (0 != error_code)
    {
        printf("Failed to lock mutex: %s\n", strerror(error_code));
    }
}

void unlockMutex(int mutex_num)
{
    int error_code = pthread_mutex_unlock(&mutexes[mutex_num]);
    if (0 != error_code)
    {
        printf("Failed to unlock mutex: %s\n", strerror(error_code));
    }
}

void *childPrintStrings(void *args)
{
    lockMutex(CHILD);
    for (int i = 1; i <= STRINGS_TO_PRINT; i++)
    {
        lockMutex(PARENT);
        printf("CHILD's THREAD string №%d\n", i);
        unlockMutex(CHILD);
        lockMutex(COMMON);
        unlockMutex(PARENT);
        lockMutex(CHILD);
        unlockMutex(COMMON);
    }
    unlockMutex(CHILD);
    return NULL;
}

void parentPrintStrings()
{
    for (int i = 1; i <= STRINGS_TO_PRINT; i++)
    {
        printf("PARENT's THREAD string №%d\n", i);
        lockMutex(COMMON);
        unlockMutex(PARENT);
        lockMutex(CHILD);
        unlockMutex(COMMON);
        lockMutex(PARENT);
        unlockMutex(CHILD);
    }
    unlockMutex(PARENT);
}

void initializeMutexes()
{
    pthread_mutexattr_t attr;
    int error_code = pthread_mutexattr_init(&attr);
    if (0 != error_code)
    {
        printf("Failed to init mutex attrs: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }

    error_code = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    if (0 != error_code)
    {
        printf("Failed to set mutex attributes type: %s", strerror(error_code));
        pthread_mutexattr_destroy(&attr);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MUTEXES_NUMBER; i++)
    {
        error_code = pthread_mutex_init(&mutexes[i], &attr);
        if (0 != error_code)
        {
            printf("Failed to initialize mutex #%d with attributes: %s\n", i, strerror(error_code));
        }
        // pthread_mutex_init(&mutexes[i], NULL);
    }
    pthread_mutexattr_destroy(&attr);
}

void createChildThread()
{
    int error_code = pthread_create(&thread, NULL, childPrintStrings, NULL);
    if (0 != error_code)
    {
        printf("Failed to create a thread: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }
}

void joinChildThread()
{
    int error_code = pthread_join(thread, NULL);
    if (0 != error_code)
    {
        printf("Failed to join a thread: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }
}

int main()
{
    initializeMutexes();

    lockMutex(PARENT);
    createChildThread();
    sleep(SECONDS_TO_SLEEP);
    parentPrintStrings();

    joinChildThread();
    destroyMutexes();
    pthread_exit(0);
}