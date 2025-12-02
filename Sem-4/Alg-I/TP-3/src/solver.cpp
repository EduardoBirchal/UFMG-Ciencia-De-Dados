#include "../include/solver.hpp"
#include "../include/utils.hpp"
#include <iostream>

using namespace std;

// construtor inicializa valores padrao
Solver::Solver() {
    global_max_size = -1;
    best_mask_global = 0;
}


// define as listas de adjacencia auxiliares
void Solver::prepare_adjacencies() {
    adj_right.assign(n_right, 0);
    adj_left_to_right.assign(n_left, 0);

    for (int i = 0; i < n_right; ++i) {
        adj_right[i] = (int)(adj[n_left + i] >> n_left);
    }
    for (int i = 0; i < n_left; ++i) {
        adj_left_to_right[i] = (int)(adj[i] >> n_left);
    }
}

// determina se o subconjunto atual e melhor com ou sem o vertice i
void Solver::update_if_maximal(int mask, int i) {
    pair<int, int> prev = sos_dp[mask ^ (1 << i)]; // melhor subconjunto sem vertice i
    pair<int, int> curr = sos_dp[mask]; // melhor subconjunto com vertice i (no caso, o valor atual)
    
    if (prev.first > curr.first) { 
        // se prev e maior, escolhe prev (pra priorizar minimalidade lexicografica)
        sos_dp[mask] = prev;
    } else if (prev.first == curr.first) { 
        // se sao iguais, escolhe o lexicograficamente menor
        // queremos o conjunto que tem o menor bit menos significativo
        int m_prev = prev.second;
        int m_curr = curr.second;
        
        if (m_prev != m_curr) {
            // encontra o bit mais baixo onde as mascaras diferem
            int diff = m_prev ^ m_curr;
            int lowest = diff & -diff;
            
            // se prev tem esse bit ligado, ele e lexicograficamente menor
            if (m_prev & lowest) sos_dp[mask] = prev;
        }
    }
    
    // se curr e maior, mantemos curr (ou seja, o melhor subconjunto é o proprio conjunto)
}

// constroi o array que contem o subconjunto indep. maximo pra cada subconjunto da direita do grafo
void Solver::build_sos_dp() {
    int size_right = 1 << n_right;
    sos_dp.assign(size_right, {0, 0});
    mask_right_all = size_right - 1;

    // inicializacao da base do dp
    // verifica quais mascaras sao conjuntos independentes validos por si so
    for (int mask = 0; mask < size_right; ++mask) {
        bool is_independent = true;
        
        // verifica conflitos internos na mascara
        for (int i = 0; i < n_right; ++i) {
            if (test_bit(mask, i)) {
                if (adj_right[i] & mask) {
                    is_independent = false;
                    break;
                }
            }
        }
        
        // se valido, armazena seu tamanho e a propria mascara
        if (is_independent) sos_dp[mask] = {count_bits(mask), mask};
        else sos_dp[mask] = {0, 0};
    }

    // propaga os maximos pra cima
    // pra cada conjunto, escolhe o melhor subconjunto valido (incluindo a si proprio)
    for (int i = 0; i < n_right; ++i) {
        for (int mask = 0; mask < size_right; ++mask) {
            // se o bit i esta ligado na mascara atual
            if (test_bit(mask, i)) {
                update_if_maximal(mask, i);
            }
        }
    }
}

// resolve o grafo da esquerda usando o da direita
void Solver::solve_left(int idx, int current_mask, int forbidden_in_right) {
    // caso base
    if (idx == n_left) {
        int allowed_in_right = mask_right_all & (~forbidden_in_right); // maior subconjunto do right compativel com o subconjunto do left atual
        pair<int, int> best_right = sos_dp[allowed_in_right]; // maior subconjunto independente em allowed_in_right 
        
        int total = count_bits(current_mask) + best_right.first; // tamanho total do conjunto independente atual
        
        // se o conjunto independente atual e maior que o max. global, ele se torna o max. global
        // nao atualiza em caso de empate pra manter o lexicograficamente menor
        // pois a recursao prioriza a inclusao de vertices menores
        if (total > global_max_size) {
            global_max_size = total;
            best_mask_global = (long long)current_mask | ((long long)best_right.second << n_left);
        }
        return;
    }
    
    // tenta incluir o vertice idx
    if ((adj[idx] & current_mask) == 0) {
        int new_mask = current_mask | (1 << idx); // add o vertice atual 
        int new_forbidden = forbidden_in_right | adj_left_to_right[idx]; // vizinhos do novo vertice em right sao proibidos
        
        solve_left(idx + 1, new_mask, new_forbidden); 
    }
    
    // nao inclui o vertice idx
    solve_left(idx + 1, current_mask, forbidden_in_right);
}

void Solver::run() {
    // chama read_graph global passando as variaveis membro como referencia
    // n, n_left, n_right e adj serao preenchidos pela funcao
    ::read_graph(n, n_left, n_right, adj);
    
    // se nao tem nenhum vertice, retorna 0
    if (n == 0) {
        cout << "0" << endl;
        return;
    }
    
    prepare_adjacencies();
    build_sos_dp();
    solve_left(0, 0, 0);
    
    cout << global_max_size << endl;
    
    bool first = true;
    for (int i = 0; i < n; ++i) {
        if ((best_mask_global >> i) & 1) {
            if (!first) cout << " ";
            cout << i;
            first = false;
        }
    }
    
    cout << endl;
}