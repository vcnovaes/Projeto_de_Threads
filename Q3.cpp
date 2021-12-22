#include <bits/stdc++.h>
#include <pthread.h>

typedef pthread_t thr_t;
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;

using namespace std;

struct MapNode { 
    int idx; 
    int valid; 
}; 
struct Args{
    int idx; 
    int target; 
    Args(int idx, int target){
        this->idx = idx ;
        this-> target = target;
    }
    Args()=default;
};

mutex_t mutex_search = PTHREAD_MUTEX_INITIALIZER;

class Graph{
    public:
        vector < vector <int> > nodes;
        vector<int> visited;
        int n_nodes; 
        
    public:
        Graph(int n_nodes){
            nodes.resize(n_nodes);
            this->n_nodes = n_nodes; 
            visited.resize(n_nodes,false);
        }
        void add_edge(int src, int dest){
            nodes[src].push_back(dest); 
            nodes[dest].push_back(src); 
        }
        int get_n_nodes(){
            return n_nodes;
        }
     
        void convert_map_to_graph(vector< vector < MapNode >> &map){
            int mp_sz = map.size();
            for(int i = 0 ; i < mp_sz ; i ++ ){
                for(int j = 0 ; j < mp_sz; j++){
                    if( i >  0){
                        if(!map[i-1][j].valid)
                            add_edge(map[i][j].idx, map[i-1][j].idx);
                    }
                    if((i+1) < mp_sz){
                        if(!map[i+1][j].valid)
                            add_edge(map[i][j].idx, map[i+1][j].idx);
                    }
                    if(j > 0){
                        if(!map[i][j-1].valid){
                            add_edge(map[i][j-1].idx, map[i][j].idx); 
                        }
                    }
                    if(j+1  < mp_sz){
                        if(!map[i][j+1].valid){
                            add_edge(map[i][j+1].idx, map[i][j].idx);
                        }
                    }
                }
            }
            
        }

        void print_map(size_t map_size){
            for (int i = 0 ; i < map_size ; i++ ){
                for(int j = i ;  (j - i) < map_size; j++ ){
                    cout << j  << ' ';
                }
                cout << '\n';
            }
        }
};

vector < vector <int> > nodes;
vector<int> visited;
int n_nodes; 
    

void* depth_first_search(void *args){
    Args param = *(Args*)args;
    int idx = param.idx;
    int obj = param.target;
    pthread_mutex_lock(&mutex_search);
    visited[idx] = true; 
    pthread_mutex_unlock(&mutex_search);
    thr_t thr_node; 
    if(idx == obj){
        std::cout << "Achou\n";
        pthread_exit(nullptr);
    }
    cout<< " " << idx  << " "; 
    for(int i  =0 ; i < nodes[idx].size() ; i++){

        if(!visited[nodes[idx][i]]){
            Args arg(nodes[idx][i],obj);
            pthread_create(&thr_node, nullptr, depth_first_search, &arg);
            pthread_join(thr_node, nullptr);
        }
    }
}
        

int main(){
    size_t map_size; 
    cin >> map_size; 
    vector < vector < MapNode > > map(map_size);
    int n = 0; 
    for (int i = 0 ; i < map_size ; i++) map[i].resize(map_size); 
    for (int i = 0 ; i < map_size ; i ++){
        for(int j = 0 ; j < map_size; j++){
            cin >> map[i][j].valid; 
            map[i][j].idx = n; 
            n++; 
        }
    }
    Graph  graph(map_size*map_size);
    cout << graph.get_n_nodes();
    graph.convert_map_to_graph(map);
    nodes = graph.nodes;
    visited = graph.visited;
    n_nodes = nodes.size();
    Args arg(0,8);
    depth_first_search(&arg);
   // graph.print_map(map_size);

    return 0 ; 
}