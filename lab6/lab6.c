#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define COEF 20000
#define MAX_STRINGS_COUNT 100

void *print_string_with_wait(void *args)
{
    usleep(COEF * strlen((char *)args));
    printf("%s", (char *)args);
    return NULL;
}

void free_strings(char **strings, int strCount)
{
    for (int i = 0; i < strCount; i++)
    {
        free(strings[i]);
    }
}

int main()
{
    char *strings[MAX_STRINGS_COUNT];
    int idx = 0;

    int readCount = 2;
    size_t strlen = 200;

    printf("ENTER STRINGS TO SORT:\n");
    while (readCount > 1)
    {
        strings[idx] = malloc(sizeof(char) * strlen);
        readCount = getline(&strings[idx], &strlen, stdin);
        idx++;

        if (readCount == 1)
        {
            idx--;
            free(strings[idx]);
        }
    }

    int strCount = idx;
    printf("#===== SORTING IN PROGRESS =====#\n");

    pthread_t threads[strCount];
    int error_code = 0;
    for (int i = 0; i < strCount; i++)
    {
        error_code = pthread_create(&threads[i], NULL, print_string_with_wait, strings[i]);
        if (0 != error_code)
        {
            printf("Failed to create new thread #%d: %s", (i + 1), strerror(error_code));
            free_strings(strings, strCount);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < strCount; i++)
    {
        error_code = pthread_join(threads[i], NULL);
        if (0 != error_code)
        {
            printf("Failed to join thread #%d: %s", (i + 1), strerror(error_code));
            free_strings(strings, strCount);
            exit(EXIT_FAILURE);
        }
        free(strings[i]);
    }

    return EXIT_SUCCESS;
}
