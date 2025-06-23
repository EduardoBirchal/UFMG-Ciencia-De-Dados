#ifndef STACK_H
#define STACK_H

#include "linkedList.hpp" 

template<class T>
class stack {
private:
    // A lista encadeada interna
    linkedList<T> list; 

public:
    // Construtor
    explicit stack(bool doDeallocateNodes = false);

    // Destrutor
    ~stack();

    // Adiciona no topo
    void push(T data);

    // Lê o topo
    T& peek();

    // Remove do topo
    T pop();

    // Verifica se vazia
    bool isEmpty() const;
};

// Construtor: inicializa a lista interna
template<class T>
stack<T>::stack(bool doDeallocateNodes): list(doDeallocateNodes) {
    // BUG CORRIGIDO: Sintaxe de inicialização de membro para composição
}

// Destrutor (usa o da lista)
template<class T>
stack<T>::~stack() {}

// Adiciona no topo (chama addToHead)
template<class T>
void stack<T>::push(T data) {
    list.addToHead(data);
}

// Lê o topo (chama readHead)
template<class T>
T& stack<T>::peek() {
    return list.readHead();
}

// Remove do topo
template<class T>
T stack<T>::pop() {
    T data = peek();
    list.remove(0); // Remove da cabeça da lista
    return data;
}

// Verifica se vazia (chama isEmpty da lista)
template<class T>
bool stack<T>::isEmpty() const {
    return list.isEmpty();
}

#endif
