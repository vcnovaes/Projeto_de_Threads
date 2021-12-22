#include <pthread.h>
#include <iostream>
#include <vector>

using namespace std;
int T, N; // T = quantidade de threads a ser utilizada
vector<bool> crivo;
pthread_mutex_t myMutex;

void *task(void *arg)
{
    int currNum = 2; // O primo atual
    while (currNum <= N)
    {
        pthread_mutex_lock(&myMutex);
        int i = currNum;
        if (crivo[currNum]) // Só faz a checagem do while dos múltiplos caso seja primo
        {
            while (i <= N) // Crivo
            {
                if (i % currNum == 0 && i != currNum)
                {
                    crivo[i] = false;
                }

                ++i;
            }
        }
        pthread_mutex_unlock(&myMutex);

        ++currNum;
    }
}

int main(int argc, char *argv[])
{
    cout << "Insira o número de threads que serão utilizadas:\n";
    cin >> T;
    system("clear");
    cout << "Insira o número:\n";
    cin >> N;

    crivo.resize(N + 1);
    crivo[0] = crivo[1] = false;

    int *threadIds[T];
    pthread_t threads[T];

    for (int i = 0; i < T; ++i)
    {
        threadIds[i] = (int *)malloc(sizeof(int)); // Criando o Id -> um array de ponteiros pois não podemos perder a ref ao Id
        *threadIds[i] = i;

        pthread_create(&threads[i], NULL, task, (void *)threadIds[i]); // Criando a thread
    }

    // Printar primos
    for (int m = 2; m <= N; ++m)
    {
        if (crivo[m])
            cout << m << endl;
    }

    return 0;
}