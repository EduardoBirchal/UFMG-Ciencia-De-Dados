#ifndef SECAO_HPP
#define SECAO_HPP

#include "stack.hpp"
#include "pacote.hpp"

// Agrupa o ID de uma seção com sua pilha de pacotes
struct Secao {
    int id;
    stack<Pacote*> pilha;

    // Construtor
    // A pilha é inicializada com 'false' para que os pacotes não sejam deletados
    Secao(int secao_id) : id(secao_id), pilha(false) {}

    // Sobrecarga do operador de comparação '<' para ordenar seções pelo ID.
    // Essencial para o uso com o método addOrdered da linkedList.
    bool operator<(const Secao& outra) const {
        return this->id < outra.id;
    }
};

// Estrutura de comparação (functor) para ponteiros de Secao.
// Permite que algoritmos e contêineres ordenados manipulem ponteiros
// para Secao com base no ID da seção, em vez do endereço de memória.
struct CompareSecaoPtr {
    bool operator()(const Secao* a, const Secao* b) const {
        // Regra de ordenação:
        // 1. Se b for nulo, nada é menor que ele. Retorna falso.
        // 2. Se a for nulo (e b não), a é considerado menor. Retorna true.
        // 3. Se ambos não forem nulos, compara seus IDs.
        if (!b) return false;
        if (!a) return true;
        return a->id < b->id;
    }
};

#endif // SECAO_HPP
