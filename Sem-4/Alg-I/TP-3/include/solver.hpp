#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include <utility>

// classe que encapsula o problema
class Solver {
public:
    // construtor
    Solver();

    // metodo principal para rodar o programa
    void run();

private:
    // variaveis do problema
    int n;
    int n_left;
    int n_right;
    long long global_max_size;
    long long best_mask_global;
    int mask_right_all;

    // vetores de dados
    std::vector<long long> adj;
    std::vector<int> adj_right;
    std::vector<int> adj_left_to_right;
    std::vector<std::pair<int, int>> sos_dp;

    // funcoes auxiliares (metodos privados)
    // read_graph removido daqui (agora em utils)
    void prepare_adjacencies();
    void update_if_maximal(int mask, int i);
    void build_sos_dp();
    void solve_left(int idx, int current_mask, int forbidden_in_right);
    
    // utilitarios de bits removidos daqui (agora em utils)
};

#endif