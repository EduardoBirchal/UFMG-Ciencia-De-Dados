#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>

// Helper para deleção condicional de nós
// Template genérico para não-ponteiros
template<typename T>
struct NodeDeleter {
    // Não faz nada
    static void destroy(T) { }
};

// Especialização para ponteiros
template<typename T>
struct NodeDeleter<T*> {
    // Deleta o ponteiro
    static void destroy(T* ptr) {
        delete ptr;
    }
};


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

    // Define o dado
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

    // Retorna o índice de um valor
    int findIndexByValue(const T& value) const;

private:
    // Pega o nó em um índice
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

        // Usa o helper para deleção condicional
        if (deallocateNodes) {
            NodeDeleter<T>::destroy(current->getData());
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
        getNode(size - 1)->setNext(newNode);
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

    listNode<T>* alvo;
    if (index == 0) {
        alvo = head;
        head = head->getNext();
    } else {
        listNode<T>* antecessorAlvo = getNode(index - 1);
        alvo = antecessorAlvo->getNext();
        antecessorAlvo->setNext(alvo->getNext());
    }

    if(deallocateNodes){
        // Usa o helper para deleção condicional
        NodeDeleter<T>::destroy(alvo->getData());
    }
    delete alvo;

    size--; 
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

// Retorna o índice de um valor
template<class T>
int linkedList<T>::findIndexByValue(const T& value) const {
    listNode<T>* current = head;
    int index = 0;
    while (current != nullptr) {
        // Assume que T suporta o operador ==
        if (current->getData() == value) {
            return index;
        }
        current = current->getNext();
        index++;
    }
    return -1; // Não encontrado
}

#endif
