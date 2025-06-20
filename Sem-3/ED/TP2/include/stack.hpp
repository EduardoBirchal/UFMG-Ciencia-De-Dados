#ifndef STACK_H
#define STACK_H

#include "linkedList.hpp" 

template<class T>
class stack {
private:
    // A cabeça da lista é o topo da pilha
    linkedList<T> list; 

public:
    // Construtor
    stack();

    // Destrutor
    ~stack();

    // Adiciona um novo elemento ao topo da pilha
    void push(T data);

    // Apenas retorna o elemento do topo sem remover ele
    T peek();

    // Remove e retorna o elemento do topo da pilha
    T pop();

    // Verifica se a pilha está vazia
    bool isEmpty();
};

// Construtor: O construtor padrão da lista já é chamado, inicializando a pilha
template<class T>
stack<T>::stack() {}

// Destrutor: O destrutor da lista é chamado automaticamente
template<class T>
stack<T>::~stack() {}

// Adiciona um novo elemento ao topo da pilha (cabeça da lista)
template<class T>
void stack<T>::push(T data) {
    list.addToHead(data);
}

// Retorna o elemento do topo sem remover ele
template<class T>
T stack<T>::peek() {
    return list.readHead();
}

// Remove e retorna o elemento do topo da pilha. Se a pilha estiver vazia, lança uma exceção
template<class T>
T stack<T>::pop() {
    T data = peek();
    list.remove(0); // Remove o antigo topo (cabeça da lista)
    return data;
}

// Verifica se a pilha está vazia
template<class T>
bool stack<T>::isEmpty() {
    return list.isEmpty();
}

#endif // STACK_H