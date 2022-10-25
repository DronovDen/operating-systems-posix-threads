#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_STRING_SIZE 80

typedef struct listNode
{
    char *str;
    struct listNode *next;
} node_t;

node_t *head;
pthread_t thread;
pthread_rwlock_t rwlock;
int isFinish = 0;

node_t *addStringNode(node_t *head, char *string)
{
    node_t *curHead = head;
    size_t strSize = strlen(string);
    if (strSize > MAX_STRING_SIZE)
    {
        curHead = addStringNode(curHead, string + MAX_STRING_SIZE);
    }
    node_t *newHead = (node_t *)malloc(sizeof(node_t));
    newHead->str = (char *)calloc(MAX_STRING_SIZE + 1, sizeof(char));
    strncpy(newHead->str, string, strSize % (MAX_STRING_SIZE + 1));
    newHead->next = curHead;
    return newHead;
}

void printList(node_t *head)
{
    node_t *cur = head;
    while (cur != NULL)
    {
        printf("%s", cur->str);
        cur = cur->next;
    }
}

void freeList(node_t *head)
{
    while (head != NULL)
    {
        node_t *next = head->next;
        free(head);
        head = next;
    }
}

void sort(node_t *head)
{
    if (head == NULL)
    {
        return;
    }
    node_t *left = head;
    node_t *right = head->next;
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->str = (char *)calloc(MAX_STRING_SIZE + 1, sizeof(char));
    while (left->next != NULL)
    {
        while (right != NULL)
        {
            if (strncmp(left->str, right->str, MAX_STRING_SIZE) > 0)
            {
                strncpy(temp->str, left->str, MAX_STRING_SIZE);
                strncpy(left->str, right->str, MAX_STRING_SIZE);
                strncpy(right->str, temp->str, MAX_STRING_SIZE);
            }
            right = right->next;
        }
        left = left->next;
        right = left->next;
    }
    free(temp->str);
    free(temp);
}

void *sortThread(void *param)
{
    while (!isFinish)
    {
        pthread_rwlock_wrlock(&rwlock);
        printf("CHILD: SORTING LIST...\n");
        sort(head);
        printf("CHILD: LIST IS SORTED\n\n");
        pthread_rwlock_unlock(&rwlock);
        sleep(5);
    }
    return NULL;
}

void startChildThread()
{
    int error_code = pthread_create(&thread, NULL, sortThread, NULL);
    if (0 != error_code)
    {
        printf("Failed to create child thread: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }
}

void parentRoutine()
{
    char buffer[MAX_STRING_SIZE + 1] = {0};
    while (1)
    {
        fgets(buffer, MAX_STRING_SIZE, stdin);
        if (!strcmp("\n", buffer))
        {
            pthread_rwlock_rdlock(&rwlock);
            printf("PARENT: PRINTING LIST...\n");
            printList(head);
            printf("PARENT: LIST PRINTED\n\n");
            pthread_rwlock_unlock(&rwlock);
        }
        else
        {
            pthread_rwlock_wrlock(&rwlock);
            head = addStringNode(head, buffer);
            pthread_rwlock_unlock(&rwlock);
        }
    }
    isFinish = 1;
}

int main(int argc, char **argv)
{
    pthread_rwlock_init(&rwlock, NULL);

    startChildThread();
    parentRoutine();

    freeList(head);
    exit(EXIT_SUCCESS);
}
