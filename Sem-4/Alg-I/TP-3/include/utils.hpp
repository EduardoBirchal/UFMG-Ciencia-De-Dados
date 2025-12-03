#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

// utilidades de bitmask
bool test_bit(int mascara, int i);
int count_bits(int mascara);

// le o input e gera o bitmask do grafo
void read_graph(int& n, int& n_left, int& n_right, std::vector<long long>& adj);

#endif