#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

// declaracoes das funcoes utilitarias
bool test_bit(int mascara, int i);
int count_bits(int mascara);

// le o grafo e preenche as variaveis passadas por referencia
void read_graph(int& n, int& n_left, int& n_right, std::vector<long long>& adj);

#endif