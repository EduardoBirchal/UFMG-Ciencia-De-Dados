#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>

// Nó da lista encadeada
template<class T> class listNode
{
private:
    T data;
    listNode *next;

public:
    // Construtor
    listNode (T inData, listNode *inNext = nullptr) { this->data = inData; this->next = inNext; }

    // Destrutor
    ~listNode () {}

    // Retorna dado
    T getData () { return this->data; }

    // Define dado
    void setData (T data) { this->data = data; }    

    // Retorna próximo nó
    listNode* getNext () { return this->next; }

    // Define próximo nó
    void setNext (listNode* next) { this->next = next; }
};

// Lista encadeada
template<class T> class linkedList {
private:
    listNode<T>* head;

public:
    // Construtor
    linkedList();

    // Destrutor
    ~linkedList();

    // Verifica se a lista está vazia
    bool isEmpty();

    // Adiciona na cabeça
    void addToHead(T data);

    // Adiciona na cauda
    void addToTail(T data);

    // Remove por índice
    bool remove(int index);

    // Lê a cabeça
    T readHead();
};


// Construtor
template<class T>
linkedList<T>::linkedList() {
    head = nullptr;
}

// Destrutor
template<class T>
linkedList<T>::~linkedList() {
    listNode<T>* current = head;
    while (current != nullptr) {
        listNode<T>* next = current->getNext();
        delete current;
        current = next;
    }
}

// Verifica se vazia
template<class T>
bool linkedList<T>::isEmpty() {
    return head == nullptr;
}

// Adiciona na cabeça
template<class T>
void linkedList<T>::addToHead(T data) {
    // Cria novo nó, aponta para a cabeça antiga
    listNode<T>* newNode = new listNode<T>(data, head);
    // Nova cabeça é o novo nó
    head = newNode;
}

// Adiciona na cauda
template<class T>
void linkedList<T>::addToTail(T data) {
    // Cria novo nó
    listNode<T>* newNode = new listNode<T>(data);

    // Se vazia, novo nó é a cabeça
    if (isEmpty()) {
        head = newNode;
    } else {
        // Senão, percorre até o fim
        listNode<T>* temp = head;
        while (temp->getNext() != nullptr) {
            temp = temp->getNext();
        }
        // Conecta o último nó
        temp->setNext(newNode);
    }
}

// Remove por índice
template<class T>
bool linkedList<T>::remove(int index) {
    // Checa se índice é inválido
    if (isEmpty() || index < 0) {
        return false;
    }

    // Remove a cabeça
    if (index == 0) {
        listNode<T>* temp = head;
        head = head->getNext();
        delete temp;
        return true;
    }

    // Percorre até o nó anterior ao alvo
    listNode<T>* current = head;
    int currentIndex = 0;
    while (current != nullptr && currentIndex < index - 1) {
        current = current->getNext();
        currentIndex++;
    }

    // Checa se índice está fora dos limites
    if (current == nullptr || current->getNext() == nullptr) {
        return false;
    }

    // Remove o nó
    listNode<T>* nodeToDelete = current->getNext();
    current->setNext(nodeToDelete->getNext());  // Desconecta
    delete nodeToDelete;                        // Libera memória
    return true;
}

// Lê a cabeça
template<class T>
T linkedList<T>::readHead() {
    if (!isEmpty()) {
        return head->getData();
    }
    // Se vazia, lança exceção
    throw std::out_of_range("Lista vazia");
}

#endif
