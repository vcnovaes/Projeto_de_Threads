#include <bits/stdc++.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
/*
 Temos de forma analoga a threads escritoras e leitoras, as consumidoras e produtoras 
 1. As threads escritoras 
    - Enquanto o banco de dados é atualizado, ou seja uma escritora está atuando, trheads leitoras
    não podem realizar leituras no banco de dados 
    - As trheads escritoras devem escreer continuamente no array em qualquer posição 
    -Acesso exclusivo a região critica usando mutex
 2. A base de dados deve ser um array
 3. As threads leitoras
    - Devem ler de qualquer posição os dados  
    - As threads leitoras não bloqueiam outra thread leitora 
//! Precisaremos de uma variavel de condição is_writing 
*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t writing = PTHREAD_COND_INITIALIZER;



class Database{
    private:
        std::vector<int> data;
        size_t size;
    public:
        Database(size_t size){
            data.resize(size);
            for(size_t i = 0 ; i < size; i++) data[i] = i; 
            this->size = size; 
        }
        void read(){
            pthread_cond_wait(&writing, &mutex); //espera que não esteja havendo escrita
            std::cout  << "Reading: " << data[rand() %size]  << '\n'; 
            visualize();
        }
        void write(){
            pthread_mutex_lock(&mutex);  //bloqueia região critica
            std::cout << "writing... \n";  
            this->data[rand()%size] = rand()%1000; 
            pthread_mutex_unlock(&mutex); // desbloqueia região critica
            pthread_cond_broadcast(&writing); // permite que todas as threads de leitura consigam ler os dados
        }
        void visualize(){
            for(int i : data){
                std::cout << i << ' ';
            }
            std::cout << std::endl; 
        }
};
const size_t DATABASE_SIZE = 100;
Database database(DATABASE_SIZE); 

void * Reader(void *thr_id); 
void * Writer(void *thr_id); 



void * Reader(void * thr_id){
    std::cout << "Reader id - " << *(int *)thr_id <<  "\n"; 
    while(true) database.read();
        //database.visualize();
    pthread_exit(nullptr); 
}


void *Writer(void * thr_id){
    std::cout << "Writer id - " << *(int *)thr_id << "\n";
    while(true)
        database.write(); 
        //database.visualize();
    pthread_exit(nullptr); 
}


int main(){
    int n_readers, n_writers; 
    std::cin >> n_readers >> n_writers; 
    pthread_t readers[n_readers], writers[n_writers];
    int *wr_ids[n_writers], *re_ids[n_readers]; 
    for(int i = 0 ; i < n_readers; i++){
        re_ids[i] = (int *)new int; 
        *re_ids[i] = i; 
        pthread_create(&readers[i], nullptr, Reader, (void * )re_ids[i]);
    }
    for(int j = 0 ; j < n_writers; j ++ ){
        wr_ids[j] = (int *)new int; 
        *wr_ids[j] = j + n_readers; 
        pthread_create(&writers[j], nullptr, Writer, (void *) wr_ids[j]);
    }
    for(int i = 0 ; i < n_readers; i++){
        pthread_join(readers[i],nullptr);
    }
    for(int j = 0 ; j < n_writers; j ++ ){
        pthread_join(writers[j],nullptr); 
    }    
    pthread_exit(nullptr);
    return 0;
}