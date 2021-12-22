#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

typedef struct
{
    int id, left, right;
} ThreadArgs;

vector<string> colors;
vector<vector<int> > colorsInt; // Cores Parsed
vector<int> grayScales;         // Escalas de cinza
int lines, columns;

void read()
{
    int lineCounter = 0;
    string line;
    fstream file;
    file.open("teste.txt", ios::in);
    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            if (lineCounter == 1)
            {
                lines = line[2] - 48;
                columns = line[0] - 48;
            }
            else if (lineCounter >= 3)
            {
                colors[lineCounter - 3] = line;
            }

            lineCounter++;
        }

        file.close();

        // Parse das cores em string
        string temp = '\0';
        colorsInt.resize(lineCounter - 3);
        for (int j = 0; j < colors.size(); ++j)
        {
            colorsInt[j].resize(3);

            temp = colors[j].substr(0, 3);
            colorsInt[j][0] = stoi(temp);

            temp = colors[j].substr(4, 3);
            colorsInt[j][1] = stoi(temp);

            temp = colors[j].substr(8, 3);
            colorsInt[j][2] = stoi(temp);
        }
    }
    else
    {
        cout << "Não foi possível abrir o arquivo\n";
    }
}

void write()
{
    fstream file;
    file.open("testeOut.txt", fstream::out);
    if (file.is_open())
    {
        for (int n = 0; n < grayScales.size(); ++n)
        {
            file << grayScales[n] << '\n';
        }

        file.close();
    }
    else
    {
        cout << "Não foi possível criar o arquivo\n";
    }
}

void *task(void *arg)
{
    ThreadArgs *arguments = (ThreadArgs *)(arg);

    for (int i = (*arguments).left; i <= (*arguments).right; ++i)
    {
        grayScales[i] = (colorsInt[i][0] * 0.30) + (colorsInt[i][1] * 0.59) + (colorsInt[i][2] * 0.11);
    }
}

int main()
{
    read();        // leitura
    int T = lines; // Para que cada thread cuide de uma linha
    int *threadIds[T];
    pthread_t threads[T];

    for (int i = 0; i < T; ++i)
    {
        threadIds[i] = (int *)malloc(sizeof(int)); // Criando o Id -> um array de ponteiros pois não podemos perder a ref ao Id
        *threadIds[i] = i;

        ThreadArgs args = {.id = i, .left = i * 3, .right = i * 3 + columns};
        pthread_create(&threads[i], NULL, task, &args); // Criando a thread
    }

    write(); // Escrever as escalas de cinza no arquivo

    return 0;
}
