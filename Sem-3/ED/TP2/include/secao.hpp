#ifndef SECAO_HPP
#define SECAO_HPP

#include "stack.hpp"
#include "pacote.hpp"

// Agrupa o ID de uma seção com sua pilha de pacotes
struct Secao {
    int id;
    stack<Pacote*> pilha;

    // Construtor
    // A pilha é inicializada com 'true' para gerenciar a memória dos ponteiros de Pacote
    Secao(int secao_id) : id(secao_id), pilha(true) {}
};

#endif // SECAO_HPP
