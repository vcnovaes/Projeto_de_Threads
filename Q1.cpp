#include <pthread.h>
#include <iostream>
#define THREAD_AMOUNT 3

using namespace std;
int counter = 0; // Declarando contador
pthread_mutex_t myMutex;

void *task(void *arg)
{
    for (int k = 0; k < 1000000; ++k)
    {
        pthread_mutex_lock(&myMutex);
        ++counter;
        if (counter == 1000000)
            break;
        pthread_mutex_unlock(&myMutex);
    }
    if (counter == 1000000)
        printf("Valor Alcancado!");
}

int main(int argc, char *argv[])
{
    int *threadIds[THREAD_AMOUNT];
    pthread_t threads[THREAD_AMOUNT];

    for (int i = 0; i < THREAD_AMOUNT; ++i)
    {
        threadIds[i] = (int *)malloc(sizeof(int)); // Criando o Id -> um array de ponteiros pois não podemos perder a ref ao Id
        *threadIds[i] = i;

        int err = pthread_create(&threads[i], NULL, task, (void *)threadIds[i]); // Criando a thread
    }

    int id = 1;
    pthread_t thread1;
    pthread_create(&thread1, NULL, task, &id);

    return 0;
}