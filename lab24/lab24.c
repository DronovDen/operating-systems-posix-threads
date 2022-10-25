#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COUNT_WIDGET 10

sem_t sem_a;
sem_t sem_b;
sem_t sem_c;
sem_t sem_widget;
sem_t sem_module;

int count_a = 0;
int count_b = 0;
int count_c = 0;

int count_module = 0;
int count_widget = 0;

pthread_t a;
pthread_t b;
pthread_t c;
pthread_t module;
pthread_t widget;

void *producer_module()
{
    while (1)
    {
        sem_wait(&sem_a);
        sem_wait(&sem_b);
        sem_post(&sem_module);
    }
}

void *producer_widget()
{
    while (1)
    {
        sem_wait(&sem_c);
        sem_wait(&sem_module);
        sem_post(&sem_widget);
    }
}

void *producer_a()
{
    while (1)
    {
        sleep(1);
        sem_post(&sem_a);
    }
}

void *producer_b()
{
    while (1)
    {
        sleep(2);
        sem_post(&sem_b);
    }
}

void *producer_c()
{
    while (1)
    {
        sleep(3);
        sem_post(&sem_c);
    }
}

void initialize_sems()
{
    sem_init(&sem_a, 0, 0);
    sem_init(&sem_b, 0, 0);
    sem_init(&sem_c, 0, 0);
    sem_init(&sem_widget, 0, 0);
    sem_init(&sem_module, 0, 0);
}

void pthread_create_CHK(pthread_t *worker, void *func)
{
    int error_code = pthread_create(worker, NULL, func, NULL);
    if (0 != error_code)
    {
        printf("Failed to create thread: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }
}

void start_producing()
{
    pthread_create_CHK(&a, producer_a);
    pthread_create_CHK(&b, producer_b);
    pthread_create_CHK(&module, producer_module);
    pthread_create_CHK(&c, producer_c);
    pthread_create_CHK(&widget, producer_widget);

    /* pthread_create(&a, NULL, producer_a, NULL);
    pthread_create(&b, NULL, producer_b, NULL);
    pthread_create(&module, NULL, producer_module, NULL);
    pthread_create(&c, NULL, producer_c, NULL);
    pthread_create(&widget, NULL, producer_widget, NULL); */

    printf("Production started!\n");
}

void sem_getvalue_CHK(sem_t *sem, int *counter)
{
    int error_code = sem_getvalue(sem, counter);
    if (0 != error_code)
    {
        printf("Failed to get semaphore value: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }
}

void get_components_count()
{
    sem_getvalue_CHK(&sem_a, &count_a);
    sem_getvalue_CHK(&sem_b, &count_b);
    sem_getvalue_CHK(&sem_c, &count_c);
    sem_getvalue_CHK(&sem_widget, &count_widget);
    sem_getvalue_CHK(&sem_module, &count_module);
}

void pthread_cancel_CHK(pthread_t *thread)
{
    int error_code = pthread_cancel(*thread);
    if (0 != error_code)
    {
        printf("Failed to cancel thread: %s", strerror(error_code));
        exit(EXIT_FAILURE);
    }
}

void stop_producing()
{
    pthread_cancel_CHK(&a);
    pthread_cancel_CHK(&b);
    pthread_cancel_CHK(&c);
    pthread_cancel_CHK(&widget);
    pthread_cancel_CHK(&module);
    printf("Production stopped!\n");
}

void print_components_count()
{
    printf("A: %d\n", count_a);
    printf("B: %d\n", count_b);
    printf("C: %d\n", count_c);
    printf("Modules: %d\n", count_module);
    printf("Widgets: %d\n", count_widget);
}

int main()
{
    initialize_sems();
    start_producing();

    while (count_widget < MAX_COUNT_WIDGET)
    {
        sleep(3);
        get_components_count();
        print_components_count();
    }

    stop_producing();

    return EXIT_SUCCESS;
}
