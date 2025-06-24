#ifndef MINHEAP_HPP
#define MINHEAP_HPP

#include <stdexcept>
#include <algorithm> // Para std::swap

// Template para Min-Heap com capacidade fixa
template<class T>
class MinHeap {
private:
    T* heap;          // Array de elementos do heap
    int capacity;     // Capacidade máxima
    int count;        // Número atual de elementos

    // --- Métodos Auxiliares ---

    // Retorna índice do pai
    int parent(int i) { return (i - 1) / 2; }

    // Retorna índice do filho esquerdo
    int leftChild(int i) { return (2 * i) + 1; }

    // Retorna índice do filho direito
    int rightChild(int i) { return (2 * i) + 2; }

    // Move um nó para cima (heapify-up)
    void heapifyUp(int index);

    // Move um nó para baixo (heapify-down)
    void heapifyDown(int index);

public:
    // Construtor
    explicit MinHeap(int maxHeapSize);

    // Destrutor
    ~MinHeap();

    // Construtor de cópia
    MinHeap(const MinHeap<T>& other);

    // Operador de atribuição
    MinHeap<T>& operator=(const MinHeap<T>& other);

    // --- Métodos Principais ---

    // Insere um novo elemento
    void insert(T key);

    // Remove e retorna o menor elemento
    T extractMin();

    // Retorna o menor elemento (sem remover)
    T getMin() const;

    // Verifica se o heap está vazio
    bool isEmpty() const;

    // Retorna o número de elementos
    int getSize() const;
    
    // Retorna a capacidade máxima
    int getCapacity() const;
};

// Construtor
template<class T>
MinHeap<T>::MinHeap(int maxHeapSize) : capacity(maxHeapSize), count(0) {
    if (capacity <= 0) {
        throw std::invalid_argument("Capacidade do heap deve ser > 0");
    }
    heap = new T[capacity];
}

// Destrutor
template<class T>
MinHeap<T>::~MinHeap() {
    delete[] heap;
}

// Construtor de cópia
template<class T>
MinHeap<T>::MinHeap(const MinHeap<T>& other) {
    capacity = other.capacity;
    count = other.count;
    heap = new T[capacity];
    for (int i = 0; i < count; ++i) {
        heap[i] = other.heap[i];
    }
}

// Operador de atribuição
template<class T>
MinHeap<T>& MinHeap<T>::operator=(const MinHeap<T>& other) {
    if (this != &other) { // Protege contra auto-atribuição
        delete[] heap; // Libera memória antiga

        capacity = other.capacity;
        count = other.count;
        heap = new T[capacity];
        for (int i = 0; i < count; ++i) {
            heap[i] = other.heap[i];
        }
    }
    return *this;
}

// Move um nó para cima
template<class T>
void MinHeap<T>::heapifyUp(int index) {
    while (index > 0 && heap[index] < heap[parent(index)]) {
        std::swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }
}

// Move um nó para baixo
template<class T>
void MinHeap<T>::heapifyDown(int index) {
    int left = leftChild(index);
    int right = rightChild(index);
    int smallest = index;

    if (left < count && heap[left] < heap[smallest]) {
        smallest = left;
    }
    if (right < count && heap[right] < heap[smallest]) {
        smallest = right;
    }

    if (smallest != index) {
        std::swap(heap[index], heap[smallest]);
        heapifyDown(smallest);
    }
}

// Insere um novo elemento
template<class T>
void MinHeap<T>::insert(T key) {
    if (count == capacity) {
        throw std::overflow_error("Capacidade maxima do heap atingida");
    }
    heap[count] = key;
    heapifyUp(count);
    count++;
}

// Remove e retorna o menor elemento
template<class T>
T MinHeap<T>::extractMin() {
    if (isEmpty()) {
        throw std::out_of_range("Heap vazio");
    }

    T root = heap[0];
    heap[0] = heap[count - 1];
    count--;
    heapifyDown(0);

    return root;
}

// Retorna o menor elemento
template<class T>
T MinHeap<T>::getMin() const {
    if (isEmpty()) {
        throw std::out_of_range("Heap vazio");
    }
    return heap[0];
}

// Verifica se o heap está vazio
template<class T>
bool MinHeap<T>::isEmpty() const {
    return count == 0;
}

// Retorna o número de elementos
template<class T>
int MinHeap<T>::getSize() const {
    return count;
}

// Retorna a capacidade máxima
template<class T>
int MinHeap<T>::getCapacity() const {
    return capacity;
}

#endif // MINHEAP_HPP
