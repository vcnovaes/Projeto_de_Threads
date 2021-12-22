#include<bits/stdc++.h>
#include <pthread.h> 
#include <queue>
#include <unordered_map>

typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t; 
typedef pthread_t thr_t;


std::vector<int> ids; //vetor para guardar os ids que é retornado a cada tarefa que é adicionada na fila 

struct Params{
    /*
    Struct resposável por armazenar os parâmetros 
    O limite que determinamos é 3 parametros, mas poderiamos ter usado um array dinâmico se quisessemos N parametros
    */
    int p1, p2, p3;
    Params(int p1=0, int p2=0, int p3=0){
        this->p1 = p1; 
        this->p2 = p2; 
        this->p3 = p3; 
    } 
    Params()=default;
};

struct Jobs{
    /*
        Struct do Jobs, um job nada mais é do que uma estrutura que guarda um ponteiro para a função a ser executada, os seus parametros 
        e seu id 
    */ 
    void * (*fun)(void*);
    Params* params; 
    int id; 

    Jobs(void * (*f)(void *), int id, Params * params = nullptr){
        fun = f;
        this->id = id;    
        this->params = params; 
    }
    Jobs() = default;
};

struct thrStatus{
    /*
        Struct que guarda o status da thread  
    */ 
    cond_t cond;  // a variavel de condição relacionado a aquela tarefa
    mutex_t mutex;  // um mutex associado a variável de condição dita anteriormente
    bool finished; // uma variavel booleana que indica se o job foi executado ou não 

    thrStatus(){
        cond = PTHREAD_COND_INITIALIZER; 
        mutex = PTHREAD_MUTEX_INITIALIZER; 
        finished = false; 
    }
};

std::queue<Jobs*> q_exec; // fila de ponteiros para jobs, trata da fila de execução 
int n_fun = 40;  // o número de funções que serão criadas

std::list<std::pair<int,int>> q_ret;  // q_ret.first = id , q_ret.second = return value

std::unordered_map<int, thrStatus*  > jobStatus;  // é um map que pega como chave o id , para acessar o status do job 

mutex_t execMUTEX = PTHREAD_MUTEX_INITIALIZER; //relaciona-se com a q_exec
mutex_t retMUTEX =  PTHREAD_MUTEX_INITIALIZER; // relaciona-se com q_ret
cond_t exec_emptyBUFF = PTHREAD_COND_INITIALIZER;

int schedule_exec(void * (funexec)(void*), void * args){ // a função que agenda a execução 
    static int count_id = 0;  // o id é uma variavel estatica que é acrescentada cada vez que é chamado o agendamento
    Jobs* job = new Jobs(funexec, count_id, (Params *)args); // contruimos um novo job com o determinado id, a função passada como paramentro e a struct de arguemnntos 
    count_id++;  // não precisamos adicionar mutex no count_id porque só teremos uma thread que vai agendar a execução 
    pthread_mutex_lock(&execMUTEX);  // precisamos de mutex aqui porque outras threads acessaram os dados da fila de execução e o jobStatus
    q_exec.push(job); 
    jobStatus.insert({job->id,new thrStatus()});
    pthread_mutex_unlock(&execMUTEX);
    pthread_cond_signal(&exec_emptyBUFF); // isso libera para a função dispatch que fica dormindo quando não existem jobs na fila 
    return count_id;
 }


// A função a ser executada 
void* funexec(void * args){
    Params params = * (Params * )args; 
    int *ret = new int  ; // = new int; 
    *ret = params.p1 * 2; 
    return (void *)ret;
}

// função que pega o resultado 
int get_result(int id){
    if(!jobStatus[id]->finished) // verfdica se o job terminou sua execução 
        pthread_cond_wait(&jobStatus[id]->cond,&jobStatus[id]->mutex); // se não ela fica dormindo 
    pthread_mutex_lock(&retMUTEX); // bloqueio da fila de retorno 
    int ret_val = -1; 
    for(std::pair<int, int> res : q_ret){ //  procura na fila de retorno o valor com o id que foi pedido 
        if(res.first == id){
            ret_val  = res.second;  
        }
    }
    q_ret.remove_if([id](std::pair <int ,int> obj){ // se o job foi achardo ele é retirado da fila 
                return id == obj.first; 
            });
    pthread_mutex_unlock(&retMUTEX); // desloqueia acesso a fila de retorno 
    
    return ret_val;
} 

// função de despacho 
void * dispatch(void * args){
    int n_thr = *(int *)args;
    while(true){
        if(q_exec.empty()){ // se a fila de execução tiver vazia ela fica dormindo 
            std::cout << "A fila esta vazia, seguem os resultados\n";
            //jobStatus.clear(); // liberar memoria
            for(int i = 0; i < n_fun; i++){
                std::cout << ids[i] << '-' << get_result(ids[i]) << std::endl; 
            }
            jobStatus.clear();   
            pthread_cond_wait(&exec_emptyBUFF, &execMUTEX);
        }
        else{ // quando a thread estiver executando 
            std::cout << "\nDespachando...\n";
            thr_t threads[n_thr]; 
            for(int i = 0 ; i < n_thr ; i++ ){
                if(!q_exec.empty()){
                    Jobs * job = q_exec.front(); 
                    int * ret_val = new int; 
                    pthread_create(&threads[i], nullptr, job->fun,(void*)job->params);
                    //execução 
                    pthread_mutex_lock(&execMUTEX);
                    q_exec.pop();
                    pthread_mutex_unlock(&execMUTEX);
                    pthread_join(threads[i], (void **)&ret_val);
                    //retorno 
                    pthread_mutex_lock(&retMUTEX);
                    std::cout << "job:" << job->id << "   ret: " << *ret_val << '\n' ;
                    q_ret.push_back({job->id, *ret_val});
                    jobStatus[job->id]->finished = true; 
                    pthread_cond_broadcast(&jobStatus[job->id]->cond);
                    pthread_mutex_unlock(&retMUTEX);
                }
            }
        }
    }
    return nullptr;
}


int main(){
    // teste 1 
    /*
        1 - thr de usuario 
        6 - func 
        2  - thr
    */   
    int n_thr = 5;
    for(int i = 0 ; i <  n_fun ; i++){
        ids.push_back(schedule_exec(funexec,new Params(i,i,i)));
    }
    thr_t thr_dispatch;
    pthread_create(&thr_dispatch, nullptr, dispatch, (void*)&n_thr);
    pthread_exit(nullptr);

    return 0; 
}