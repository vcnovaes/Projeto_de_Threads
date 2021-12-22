#include <pthread.h>
#include <iostream>
#include <bits/stdc++.h>

typedef pthread_t thr_t;
typedef pthread_mutex_t mut_t; 
typedef pthread_cond_t cond_t;

/***
 *    thr_mon russa,  thr_pass , thr_carr 
 *    fila de mon_russa
 *    cpc 10 passenger  
 * 
 * 
*/
struct Passenger{
    
    thr_t thread; 
    bool can_ride; 
    cond_t cond_ride;
    int id; 
    void * (* fun)(void * );

    Passenger(void * (*routine)(void * )){
        can_ride = true; 
        cond_ride = PTHREAD_COND_INITIALIZER;
        fun = routine; 
    }
    Passenger(){
        Passenger(nullptr);
    }
}; 

std::vector<Passenger> passengers(20); 

int pass_id = 0; 
void * example_fun(void * args){
    std::cout << "Hi, i am passenger" << '\n'; 
}

void init_passengers(){
    for(Passenger & passenger : passengers){
        passenger.fun = example_fun;
        passenger.can_ride = true;
        passenger.id = pass_id;
        pass_id++;
    }
    std::cout << "inicializando... \n";
}

std::queue<Passenger> q_pass;

mut_t mutex_q_pass = PTHREAD_MUTEX_INITIALIZER;  
cond_t cond_pre_ride = PTHREAD_COND_INITIALIZER; 


void schedule_ride(Passenger &passenger){
    if(!passenger.can_ride){
        pthread_cond_wait(&passenger.cond_ride, &mutex_q_pass); 
    }
    if(q_pass.size() >= 10){
        pthread_cond_wait(&cond_pre_ride, &mutex_q_pass); 
    }
    pthread_mutex_lock(&mutex_q_pass);  // trylock? 
    q_pass.push(passenger);
    pthread_mutex_unlock(&mutex_q_pass);
}

mut_t mutex_car = PTHREAD_MUTEX_INITIALIZER;
void *  Ride(void * thr_id ){
    pthread_mutex_lock(&mutex_car);
    size_t n_pass = 10; 
    std::vector<Passenger> ride; 
    // a cada volta pegaremos os n_pass primeiros itens da nossa fila 
    if(q_pass.size() >=n_pass){
        pthread_mutex_lock(&mutex_q_pass);
        std::cout << "Current ride..."; 
        for(size_t i = 0 ; i < q_pass.size() ; i++){
            if(!q_pass.empty()){
                ride.push_back(q_pass.front());
                std::cout << ride[i].id << ' '; 
                q_pass.pop(); 
            }
        }
        std::cout << std::endl ; 
        
        pthread_mutex_unlock(&mutex_q_pass);
        for(int i = 0 ; i < ride.size(); i++){
            pthread_create(&ride[i].thread, nullptr, ride[i].fun, nullptr); 
            std::cout << "Riding " << ride[i].id << '\n';
            pthread_join(ride[i].thread, nullptr); 
        }   
        for(Passenger & pass : passengers){
            pass.can_ride = !pass.can_ride; 
            pthread_cond_broadcast(&pass.cond_ride);
        }
        
    }
    ride.clear();
    pthread_mutex_unlock(&mutex_car);
}



void *  rollerCoaster(void * args){
    uint8_t rides_limit = 10;
    thr_t carr; 
    pthread_create(&carr, nullptr, Ride, nullptr); 
    pthread_join(carr,nullptr);
}  

void * scheduler(void * args){
    for(Passenger & passenger: passengers){
        schedule_ride(passenger);
    }
}

int main(){
    init_passengers(); 
    thr_t thr_roller_coaster, thr_schedule; 
    for(int i = 0 ; i < 10 ; i++){
    
        pthread_create(&thr_schedule, nullptr, &scheduler, nullptr);
    
        pthread_create(&thr_roller_coaster, nullptr, &rollerCoaster,nullptr);
    
        pthread_join(thr_roller_coaster, nullptr);
    }
    pthread_exit(nullptr);
    return 0;
}