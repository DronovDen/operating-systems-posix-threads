#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define THREADS_NUMBER 4
#define ARRAY_LENGTH 16
#define OFFSET 4

void *print_lines(void *arg)
{
    if (arg == NULL)
    {
        printf("Expexted not NULL argument!");
        return NULL;
    }

    pthread_t self_id = pthread_self();
    printf("Thread id: %lld\n", (long long)self_id);

    char **lines = (char **)arg;
    for (char **line = lines; NULL != *line; line++)
    {
        printf("%s\n", *line);
    }
    return NULL;
}

int main()
{
    pthread_t threads[THREADS_NUMBER];
    char *strings[ARRAY_LENGTH] = {"Thread #1", "Thread #1", "Thread #1", NULL, "Thread #2", "Thread #2", "Thread #2", NULL,
                                   "Thread #3", "Thread #3", "Thread #3", NULL, "Thread #4", "Thread #4", "Thread #4", NULL};

    int error_code = EXIT_SUCCESS;
    int count_created_threads = 0;
    for (int i = 0; i < THREADS_NUMBER; ++i)
    {
        error_code = pthread_create(&threads[i], NULL, print_lines, &strings[i * OFFSET]);
        if (EXIT_SUCCESS != error_code)
        {
            printf("Failed to spawn thread: %s", strerror(error_code));
            break;
        }
        count_created_threads += 1;
    }

    for (int j = 0; j < count_created_threads; ++j)
    {
        error_code = pthread_join(threads[j], NULL);
        if (EXIT_SUCCESS != error_code)
        {
            printf("Failed to join thread #%d: %s\n", j, strerror(error_code));
        }
    }

    pthread_exit(NULL);
    return EXIT_SUCCESS;
}