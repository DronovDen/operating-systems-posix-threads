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
    pthread_mutex_t mutex;
    struct listNode *next;
} node_t;

node_t *head;
pthread_t thread;
pthread_mutex_t listMutex;
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
    pthread_mutex_init(&(newHead->mutex), NULL);
    pthread_mutex_lock(&(newHead->mutex));
    newHead->str = (char *)calloc(MAX_STRING_SIZE + 1, sizeof(char));
    strncpy(newHead->str, string, strSize % (MAX_STRING_SIZE + 1));
    newHead->next = curHead;
    pthread_mutex_unlock(&(newHead->mutex));
    return newHead;
}

void printList(node_t *head)
{
    node_t *cur = head;
    node_t *prev;
    while (cur != NULL)
    {
        pthread_mutex_lock(&(cur->mutex));
        printf("%s", cur->str);
        prev = cur;
        cur = cur->next;
        pthread_mutex_unlock(&(prev->mutex));
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
    pthread_mutex_lock(&(head->mutex));
    node_t *left = head;
    node_t *right = head->next;
    node_t *prevLeft = left;
    node_t *prevRight = right;
    pthread_mutex_unlock(&(head->mutex));

    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->str = (char *)calloc(MAX_STRING_SIZE + 1, sizeof(char));

    pthread_mutex_init(&(temp->mutex), NULL);
    while (left->next != NULL)
    {
        pthread_mutex_lock(&(left->mutex));
        while (right != NULL)
        {
            pthread_mutex_lock(&(right->mutex));
            pthread_mutex_lock(&(temp->mutex));
            if (strncmp(left->str, right->str, MAX_STRING_SIZE) > 0)
            {
                strncpy(temp->str, left->str, MAX_STRING_SIZE);
                strncpy(left->str, right->str, MAX_STRING_SIZE);
                strncpy(right->str, temp->str, MAX_STRING_SIZE);
            }
            pthread_mutex_unlock(&(temp->mutex));
            prevRight = right;
            right = right->next;
            pthread_mutex_unlock(&(prevRight->mutex));
        }
        prevLeft = left;
        left = left->next;
        right = left->next;
        pthread_mutex_unlock(&(prevLeft->mutex));
        pthread_mutex_unlock(&(left->mutex));
    }
    pthread_mutex_destroy(&(temp->mutex));
    free(temp->str);
    free(temp);
}

void *sortThread(void *param)
{
    while (!isFinish)
    {
        pthread_mutex_lock(&listMutex);
        printf("CHILD: SORTING LIST...\n");
        sort(head);
        printf("CHILD: LIST IS SORTED\n\n");
        pthread_mutex_unlock(&listMutex);
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
        pthread_mutex_lock(&listMutex);
        if (!strcmp("\n", buffer))
        {
            printf("PARENT: PRINTING LIST...\n");
            printList(head);
            printf("PARENT: LIST PRINTED\n\n");
        }
        else
        {
            printf("PARENT: ADDING NODE...\n");
            head = addStringNode(head, buffer);
            printf("PARENT: NODE ADDED SUCCESSFULL\n\n");
        }
        pthread_mutex_unlock(&listMutex);
    }
    isFinish = 1;
}

int main(int argc, char **argv)
{
    pthread_mutex_init(&listMutex, NULL);

    for (int i = 0; i < 10; i++)
    {
        startChildThread();
    }

    parentRoutine();

    freeList(head);
    exit(EXIT_SUCCESS);
}
