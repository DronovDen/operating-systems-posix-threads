#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define LINES_COUNT 10

void *linesPrint(void *arg)
{
    if (arg == NULL)
    {
        printf("Expexted not NULL argument!");
        return NULL;
    }

    char *prefix = (char *)arg;
    for (int i = 0; i < LINES_COUNT; i++)
    {
        printf("%s line №%d\n", prefix, i);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t thread;

    int err = pthread_create(&thread, NULL, linesPrint, "child");
    if (err)
    {
        printf("Failed to create a thread: %s\n", strerror(err));
        return err;
    }

    linesPrint("parent");

    // pthread_exit(NULL);
    sleep(1);
    return EXIT_SUCCESS;
}