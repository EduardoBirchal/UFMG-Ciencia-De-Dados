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
};

#endif // SECAO_HPP
