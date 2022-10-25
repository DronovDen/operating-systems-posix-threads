#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define SECONDS_TO_WAIT 2

void *printLines(void *param)
{
    while (1)
    {
        pthread_testcancel();
        printf("Hello! I'm a %s\n", (char *)param);
        // write(STDOUT_FILENO, "Child\n", 6);
    }
    return NULL;
}

int main()
{
    pthread_t thread;
    int error_code = pthread_create(&thread, NULL, printLines, "Child");
    if (error_code != EXIT_SUCCESS)
    {
        printf("Failed to spawn a thread: %s", strerror(error_code));
        return EXIT_FAILURE;
    }

    sleep(SECONDS_TO_WAIT);
    printf("Parent: Trying to cancel child thread\n");
    error_code = pthread_cancel(thread);

    if (error_code != EXIT_SUCCESS)
    {
        printf("Failed to cancel thread: %s", strerror(error_code));
        return EXIT_FAILURE;
    }

    error_code = pthread_join(thread, NULL);

    if (error_code != EXIT_SUCCESS)
    {
        printf("Failed to join a thread: %s", strerror(error_code));
        return EXIT_FAILURE;
    }

    printf("Parent: Successfully cancelled child thread\n");
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}