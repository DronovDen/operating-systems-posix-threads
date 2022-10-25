#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define SECONDS_TO_WAIT 2

void cancel_handler(void *arg)
{
    printf("%s: I am being cancelled!\n", (char *)arg);
}

void *printLines(void *param)
{
    pthread_cleanup_push(cancel_handler, "Child");
    while (true)
    {
        pthread_testcancel();
        printf("Hello! I'm a %s\n", (char *)param);
    }
    pthread_cleanup_pop(0); // 0 - при извлечении не исполнять обработчик
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
