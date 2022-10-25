#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#define STRINGS_TO_PRINT 5

pthread_mutex_t mutex;
pthread_cond_t conditionalVariable;
bool parentPrinted = false;

void *childRoutine(void *args)
{
    for (int i = 1; i <= STRINGS_TO_PRINT; ++i)
    {
        pthread_mutex_lock(&mutex);

        // Spurious wakeups may occur --> thats why we need to recheck for variable changes
        while (!parentPrinted)
        {
            printf("child inside while loop\n");
            pthread_cond_wait(&conditionalVariable, &mutex);
        }
        printf("CHILD's THREAD string №%d\n\n", i);
        parentPrinted = false;
        pthread_cond_signal(&conditionalVariable);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void mainRoutine()
{
    for (int i = 1; i <= STRINGS_TO_PRINT; ++i)
    {
        pthread_mutex_lock(&mutex);
        while (parentPrinted)
        {
            printf("parent inside while loop\n");
            pthread_cond_wait(&conditionalVariable, &mutex);
        }
        printf("PARENT's THREAD string №%d\n\n", i);
        parentPrinted = true;
        pthread_cond_signal(&conditionalVariable);
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&conditionalVariable, NULL);

    pthread_t childThread;

    int error_code = pthread_create(&childThread, NULL, childRoutine, NULL);
    if (0 != error_code)
    {
        printf("Failed to create a thread: %s", strerror(error_code));
    }

    mainRoutine();

    // join???

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionalVariable);
    pthread_exit(0);
}