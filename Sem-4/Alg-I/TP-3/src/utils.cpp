#include "utils.hpp"
#include <iostream>

using namespace std;

// implementacao de test_bit
bool test_bit(int mascara, int i) {
    return (mascara & (1 << i)) != 0;
}

// implementacao de count_bits
int count_bits(int mascara) {
    int contador = 0;
    while (mascara > 0) {
        if (mascara & 1) contador++;
        mascara >>= 1;
    }
    return contador;
}

// implementacao de read_graph
void read_graph(int& n, int& n_left, int& n_right, vector<long long>& adj) {
    int m;
    // se falhar a leitura, retorna
    if (!(cin >> n >> m)) return;

    // calcula as metades
    n_left = n / 2;
    n_right = n - n_left;
    
    // redimensiona e limpa a lista de adjacencia
    adj.assign(n, 0);

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u] |= (1LL << v);
        adj[v] |= (1LL << u);
    }
}