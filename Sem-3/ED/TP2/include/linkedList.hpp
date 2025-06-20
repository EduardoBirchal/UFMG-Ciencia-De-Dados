#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>

template<class T> class listNode
{
private:
    T data;
    listNode *next;

public:
    // Construtor
    listNode (T inData, listNode *inNext = nullptr) { this->data = inData; this->next = inNext; };

    // Destrutor
    ~listNode () {};

    // Retorna os dados
    T getData () { return this->data; }

    // Escreve novos dados
    void setData (T data) { this->data = data; }    

    // Retorna um pointer pro próximo nó
    listNode* getNext () { return this->next; }

    // Define o próximo nó
    void setNext (listNode* next) { this->next = next; }
};


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

    // Adiciona um novo nó com o dado fornecido ao início da lista
    void addToHead(T data);

    // Adiciona um novo nó com o dado fornecido ao fim da lista
    void addToTail(T data);

    // Remove um nó em um índice específico
    bool remove(int index);

    // Lê os conteúdos da cabeça
    T readHead();
};


// Construtor: Inicializa uma lista vazia
template<class T>
linkedList<T>::linkedList() {
    head = nullptr;
}

// Destrutor: Libera toda a memória alocada pros nós da lista
template<class T>
linkedList<T>::~linkedList() {
    listNode<T>* current = head;
    while (current != nullptr) {
        listNode<T>* next = current->getNext();
        delete current;
        current = next;
    }
}

// Verifica se a lista está vazia
template<class T>
bool linkedList<T>::isEmpty() {
    return head == nullptr;
}

// Adiciona um novo nó com o dado fornecido no início da lista
template<class T>
void linkedList<T>::addToHead(T data) {
    // Cria um novo nó, fazendo ele apontar para a cabeça atual
    listNode<T>* newNode = new listNode<T>(data, head);

    // A cabeça da lista agora é o novo nó
    head = newNode;
}

// Adiciona um novo nó com o dado fornecido no final da lista
template<class T>
void linkedList<T>::addToTail(T data) {
    // Cria um novo nó usando o construtor
    listNode<T>* newNode = new listNode<T>(data);

    // Se a lista estiver vazia, o novo nó se torna a cabeça
    if (isEmpty()) {
        head = newNode;
    } else {
        // Caso contrário, percorre até o final da lista
        listNode<T>* temp = head;
        while (temp->getNext() != nullptr) {
            temp = temp->getNext();
        }
        // Conecta o último nó ao novo nó
        temp->setNext(newNode);
    }
}

// Remove um nó em um índice específico
template<class T>
bool linkedList<T>::remove(int index) {
    // Verifica se o índice é inválido ou se a lista está vazia
    if (isEmpty() || index < 0) {
        return false;
    }

    // Se o nó é a cabeça, remove ele e atualiza a cabeça
    if (index == 0) {
        listNode<T>* temp = head;
        head = head->getNext();
        delete temp;
        return true;
    }

    // Senão, percorre até o nó anterior ao que será removido
    listNode<T>* current = head;
    int currentIndex = 0;
    
    // Para no nó logo antes do índice alvo
    while (current != nullptr && currentIndex < index - 1) {
        current = current->getNext();
        currentIndex++;
    }

    // Verifica se o índice está fora dos limites
    if (current == nullptr || current->getNext() == nullptr) {
        return false;
    }

    // Remove o nó 
    listNode<T>* nodeToDelete = current->getNext();
    current->setNext(nodeToDelete->getNext());  // Desconecta o nó
    delete nodeToDelete;                        // Libera sua memória
    return true;
}

// Lê e retorna o dado do nó da cabeça
template<class T>
T linkedList<T>::readHead() {
    if (!isEmpty()) {
        return head->getData();
    }

    // Se a lista estiver vazia, lança uma exceção
    throw std::out_of_range("Lista vazia");
}

#endif