#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define TOTAL_STEPS_NUM 200000000

typedef struct threadArguments
{
    int startIndex;
    int elemsNum;
    double result;
} threadArguments_t;

void *countPartPI(void *arg)
{
    threadArguments_t *threadParams = (threadArguments_t *)arg;
    double pi = 0.0;
    int numSteps = threadParams->elemsNum;
    for (int i = threadParams->startIndex; i < numSteps + threadParams->startIndex; ++i)
    {
        pi += 1.0 / (i * 4.0 + 1.0);
        pi -= 1.0 / (i * 4.0 + 3.0);
    }
    pi *= 4;
    threadParams->result = pi;
    int threadNum = threadParams->startIndex / threadParams->elemsNum + 1;
    printf("Thread #%d done %d steps with partial result %.10f\n", threadNum, numSteps, pi);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Wrong argument amount: %d\n", argc);
        return EXIT_FAILURE;
    }

    int pthreadCount = atoi(argv[1]);
    if (pthreadCount <= 0)
    {
        printf("Number of threads must be positive number!\n");
        return EXIT_FAILURE;
    }

    pthread_t *threads = (pthread_t *)malloc(pthreadCount * sizeof(pthread_t));
    if (NULL == threads)
    {
        printf("Failed to allocate memory for theads!");
        return EXIT_FAILURE;
    }

    threadArguments_t *argsForThread = (threadArguments_t *)calloc(pthreadCount, sizeof(threadArguments_t));
    if (NULL == argsForThread)
    {
        printf("Failed to allocate memory for theads arguments structure!");
        free(threads);
        return EXIT_FAILURE;
    }

    argsForThread[0].startIndex = 0;

    int error_code = EXIT_SUCCESS;
    for (int i = 0; i < pthreadCount; ++i)
    {
        if (i > 0)
        {
            argsForThread[i].startIndex = argsForThread[i - 1].startIndex + argsForThread[i - 1].elemsNum;
        }

        argsForThread[i].elemsNum = (TOTAL_STEPS_NUM / pthreadCount);
        if ((error_code = pthread_create(&threads[i], NULL, countPartPI, &argsForThread[i])))
        {
            printf("Failed to spawn thread: %s\n", strerror(error_code));
        }
    }

    double pi = 0;
    for (int i = 0; i < pthreadCount; ++i)
    {
        if ((error_code = pthread_join(threads[i], NULL)))
        {
            printf("Failed to join thread №%d: %s\n", i, strerror(error_code));
        }
        pi += argsForThread[i].result;
    }
    printf("PI = %.10f\n", pi);
    free(threads);
    free(argsForThread);
    return EXIT_SUCCESS;
}