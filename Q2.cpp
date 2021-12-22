// Divide em N partes
// Checa de as partes tão ordenadas
// se a parte não ta ordenada -> bool = false
// se ta ordenada -> bool = true
// Checa os limites
#include <pthread.h>
#include <iostream>

#define ARRAY_SIZE 10
// #define THREAD_AMOUNT 3
bool isOrdered = true;
int array[ARRAY_SIZE] = {4, 6, 3, 8, 13, 53, 4, 33, 50, 77};

typedef struct
{
    int left, right;
} Pair;

void *checkOrder(void *args)
{
    Pair *argsPair = (Pair *)args;

    // Checando se o array está em ordem decrescente
    for (int k = (*argsPair).left; k < (*argsPair).right && isOrdered; ++k)
    {
        if (array[k] > array[k + 1])
            isOrdered = false;
    }

    // Checando com o sucessor do ultimo elem da partição
    if ((*argsPair).right < 9 && array[(*argsPair).right] > array[(*argsPair).right + 1])
    {
        isOrdered = false;
    }

    // Checando com o antecessor do ultimo elem da partição
    if ((*argsPair).left > 0 && array[(*argsPair).left - 1] > array[(*argsPair).left])
    {
        isOrdered = false;
    }
}

int main(int argc, char *argv[])
{
    int THREAD_AMOUNT;
    scanf("%d", &THREAD_AMOUNT);

    int *threadIds[THREAD_AMOUNT];
    pthread_t threads[THREAD_AMOUNT];

    int partsSize = ARRAY_SIZE / THREAD_AMOUNT;

    // Criando as threads
    for (int i = 0; i < THREAD_AMOUNT && isOrdered; ++i)
    {
        threadIds[i] = (int *)malloc(sizeof(int)); // Criando o Id -> um array de ponteiros pois não podemos perder a ref ao Id
        *threadIds[i] = i;

        // Checando se já achou que está desordenado
        if (!isOrdered)
        {
            printf("Not Ordered!");
        }

        if (i == THREAD_AMOUNT - 1 && isOrdered) // Caso seja a ultima thread, botamos os elementos que sobram
        {
            Pair args = {.left = i * partsSize, .right = i * partsSize + partsSize - 1 + (ARRAY_SIZE % THREAD_AMOUNT)};
            pthread_create(&threads[i], NULL, checkOrder, &args); // Criando a thread
        }
        else if (isOrdered)
        {
            Pair args = {.left = i * partsSize, .right = i * partsSize + partsSize - 1};
            pthread_create(&threads[i], NULL, checkOrder, &args); // Criando a thread
        }
    }

    return 0;
}