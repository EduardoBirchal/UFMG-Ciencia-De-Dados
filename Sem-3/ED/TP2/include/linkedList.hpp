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
    listNode (T inData, listNode *inNext = nullptr) : data(inData), next(inNext) {}

    // Destrutor
    ~listNode () {}

    // Retorna referência ao dado
    T& getData () { return this->data; }

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
    int size;
    bool deallocateNodes; // Flag para desalocar dados se T for ponteiro

public:
    // Construtor
    explicit linkedList(bool doDeallocateNodes = false);

    // Destrutor
    ~linkedList();

    // Verifica se a lista está vazia
    bool isEmpty() const;

    // Retorna o tamanho da lista
    int getSize() const;

    // Adiciona na cabeça
    void addToHead(T data);

    // Adiciona na cauda
    void addToTail(T data);

    // Remove por índice
    bool remove(int index);

    // Lê a cabeça
    T& readHead();

    // Lê em um índice
    T& readIndex(int index);

private:
    // Pega o nó em um índice (método auxiliar)
    listNode<T>* getNode(int index);
};

// Construtor
template<class T>
linkedList<T>::linkedList(bool doDeallocateNodes) 
    : head(nullptr), size(0), deallocateNodes(doDeallocateNodes) {
}

// Destrutor
template<class T>
linkedList<T>::~linkedList() {
    listNode<T>* current = head;
    while (current != nullptr) {
        listNode<T>* next = current->getNext();

        // Se a flag estiver ativa e T for um ponteiro, deleta o dado
        if (deallocateNodes) {
            delete current->getData();
        }

        delete current;
        current = next;
    }
}

// Verifica se vazia
template<class T>
bool linkedList<T>::isEmpty() const {
    return head == nullptr;
}

// Retorna o tamanho
template<class T>
int linkedList<T>::getSize() const {
    return this->size;
}

// Adiciona na cabeça
template<class T>
void linkedList<T>::addToHead(T data) {
    listNode<T>* newNode = new listNode<T>(data, head);
    head = newNode;
    size++;
}

// Adiciona na cauda
template<class T>
void linkedList<T>::addToTail(T data) {
    listNode<T>* newNode = new listNode<T>(data);
    if (isEmpty()) {
        head = newNode;
    } else {
        listNode<T>* temp = head;
        while (temp->getNext() != nullptr) {
            temp = temp->getNext();
        }
        temp->setNext(newNode);
    }
    size++;
}

// Pega o nó em um índice
template<class T>
listNode<T>* linkedList<T>::getNode(int index) {
    if (isEmpty() || index < 0 || index >= size) {
        throw std::out_of_range("Indice invalido ou lista vazia");
    }
    listNode<T>* current = head;
    int currentIndex = 0;
    while (currentIndex < index) {
        current = current->getNext();
        currentIndex++;
    }
    return current;
}

// Remove por índice
template<class T>
bool linkedList<T>::remove(int index) {
    if (isEmpty() || index < 0 || index >= size) return false;

    if (index == 0) {
        listNode<T>* temp = head;
        head = head->getNext();
        delete temp;
    } else {
        listNode<T>* antecessorAlvo = getNode(index - 1);
        listNode<T>* alvo = antecessorAlvo->getNext();
        antecessorAlvo->setNext(alvo->getNext());
        delete alvo;
    }

    size--; // BUG CRÍTICO CORRIGIDO: decrementar o tamanho
    return true;
}

// Lê a cabeça
template<class T>
T& linkedList<T>::readHead() {
    if (!isEmpty()) {
        return head->getData();
    }
    throw std::out_of_range("Lista vazia");
}

// Lê em um índice
template<class T>
T& linkedList<T>::readIndex(int index) {
    return getNode(index)->getData();
}

#endif
