#include<bits/stdc++.h>
#include <pthread.h> 
#include <queue>
#include <unordered_map>

typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t; 
typedef pthread_t thr_t;


std::vector<int> ids;

struct Params{
    int p1, p2, p3;
    Params(int p1=0, int p2=0, int p3=0){
        this->p1 = p1; 
        this->p2 = p2; 
        this->p3 = p3; 
    } 
    Params()=default;
};

struct Jobs{
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
    cond_t cond; 
    mutex_t mutex; 
    bool finished;

    thrStatus(){
        cond = PTHREAD_COND_INITIALIZER; 
        mutex = PTHREAD_MUTEX_INITIALIZER; 
        finished = false; 
    }
};

std::queue<Jobs*> q_exec;
int n_fun = 40; 
std::list<std::pair<int,int>> q_ret;  // q_ret.first = id , q_ret.second = return value

std::unordered_map<int, thrStatus*  > jobStatus; 

mutex_t execMUTEX = PTHREAD_MUTEX_INITIALIZER; //relaciona-se com a q_exec
mutex_t retMUTEX =  PTHREAD_MUTEX_INITIALIZER; // relaciona-se com q_ret
cond_t exec_emptyBUFF = PTHREAD_COND_INITIALIZER;

int schedule_exec(void * (funexec)(void*), void * args){
    static int count_id = 0; 
    Jobs* job = new Jobs(funexec, count_id, (Params *)args);
    count_id++; 
    pthread_mutex_lock(&execMUTEX); 
    q_exec.push(job);
    jobStatus.insert({job->id,new thrStatus()});
    pthread_mutex_unlock(&execMUTEX);
    pthread_cond_signal(&exec_emptyBUFF);
    
    return count_id;
 }

void* funexec(void * args){
    Params params = * (Params * )args; 
    int *ret = new int  ; // = new int; 
    *ret = params.p1 * 2; 
    return (void *)ret;
}

/// 

int get_result(int id){
    if(!jobStatus[id]->finished)
        pthread_cond_wait(&jobStatus[id]->cond,&jobStatus[id]->mutex);
    pthread_mutex_lock(&retMUTEX);
    int ret_val = -1; 
    for(std::pair<int, int> res : q_ret){
        if(res.first == id){
            ret_val  = res.second; 
        }
    }
    q_ret.remove_if([id](std::pair <int ,int> obj){
                return id == obj.first; 
            });
    pthread_mutex_unlock(&retMUTEX);
    
    return ret_val;
} 


void * dispatch(void * args){
    int n_thr = *(int *)args;
    while(true){
        if(q_exec.empty()){
            std::cout << "A fila esta vazia, seguem os resultados\n";
            //jobStatus.clear(); // liberar memoria
            for(int i = 0; i < n_fun; i++){
                std::cout << ids[i] << '-' << get_result(ids[i]) << std::endl; 
            }
            jobStatus.clear();   
            pthread_cond_wait(&exec_emptyBUFF, &execMUTEX);
        }
        else{
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