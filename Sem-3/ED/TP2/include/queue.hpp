#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "linkedList.hpp"


template<class T>
class Queue {
private:
    // A fila é implementada em termos de uma lista encadeada
    linkedList<T> list;

public:
    // Construtor
    explicit Queue(bool doDeallocateNodes = false);

    // Destrutor
    ~Queue();

    // Adiciona um elemento ao final da fila
    void enqueue(T data);

    // Remove e retorna o elemento do início da fila
    T dequeue();

    // Apenas retorna o elemento do início sem removê-lo
    T& peek();

    // Verifica se a fila está vazia
    bool isEmpty() const;
};

// Construtor: inicializa a lista interna
template<class T>
Queue<T>::Queue(bool doDeallocateNodes) : list(doDeallocateNodes) {
    // A lista já é inicializada corretamente pelo seu construtor
}

// Destrutor (usa o da lista)
template<class T>
Queue<T>::~Queue() {}

// Adiciona um elemento ao final da fila
template<class T>
void Queue<T>::enqueue(T data) {
    // Adicionar na cauda da lista é a operação de enfileirar
    list.addToTail(data);
}

// Remove e retorna o elemento do início da fila
template<class T>
T Queue<T>::dequeue() {
    // Pega o dado da cabeça antes de remover
    T data = list.readHead();
    // Remove o nó da cabeça da lista
    list.remove(0);
    return data;
}

// Apenas retorna o elemento do início sem removê-lo
template<class T>
T& Queue<T>::peek() {
    // readHead já faz o que é necessário: lê o início ou lança exceção
    return list.readHead();
}

// Verifica se a fila está vazia
template<class T>
bool Queue<T>::isEmpty() const {
    return list.isEmpty();
}

#endif 
