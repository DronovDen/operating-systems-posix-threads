#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define STRINGS_TO_PRINT 10

int main()
{
    sem_t *semParent = sem_open("/ParentSemaphore", O_CREAT, 0600, 1);
    sem_t *semChild = sem_open("/ChildSemaphore", O_CREAT, 0600, 0);

    switch (fork())
    {
    case -1:
        perror("Fork failed");
        // sem_destroy(semParent);
        // sem_destroy(semChild);
        exit(EXIT_FAILURE);
    case 0:
        for (int i = 1; i <= STRINGS_TO_PRINT; ++i)
        {
            sem_wait(semChild);
            printf("CHILD string #%d\n", i);
            sem_post(semParent);
        }
        break;
    default:
        for (int i = 1; i <= STRINGS_TO_PRINT; ++i)
        {
            sem_wait(semParent);
            printf("PARENT string #%d\n", i);
            sem_post(semChild);
        }
        break;
    }

    sem_close(semParent);
    sem_close(semChild);

    return EXIT_SUCCESS;
}