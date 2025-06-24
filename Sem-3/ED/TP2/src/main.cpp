#include <iostream>
#include "minheap.hpp"

// Função para imprimir o estado do heap
void printHeapStatus(MinHeap<int>& heap) {
    std::cout << "Tamanho atual do heap: " << heap.getSize() << std::endl;
    if (!heap.isEmpty()) {
        std::cout << "Menor elemento (getMin): " << heap.getMin() << std::endl;
    } else {
        std::cout << "O heap esta vazio." << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;
}

int main() {
    std::cout << "--- Iniciando Teste do MinHeap ---" << std::endl;

    // 1. Cria um MinHeap com capacidade para 10 inteiros
    MinHeap<int> heap(10);
    std::cout << "Heap criado com capacidade para " << heap.getCapacity() << " elementos." << std::endl;
    printHeapStatus(heap);

    // 2. Insere elementos no heap
    std::cout << "\n--- Teste de Insercao ---" << std::endl;
    heap.insert(10);
    std::cout << "Inserido: 10" << std::endl;
    printHeapStatus(heap);

    heap.insert(4);
    std::cout << "Inserido: 4" << std::endl;
    printHeapStatus(heap);

    heap.insert(15);
    std::cout << "Inserido: 15" << std::endl;
    printHeapStatus(heap);

    heap.insert(20);
    std::cout << "Inserido: 20" << std::endl;
    printHeapStatus(heap);
    
    heap.insert(1);
    std::cout << "Inserido: 1" << std::endl;
    printHeapStatus(heap);

    // 3. Testa a extração do mínimo
    std::cout << "\n--- Teste de Extracao ---" << std::endl;
    std::cout << "Extraindo elementos em ordem:" << std::endl;
    while (!heap.isEmpty()) {
        int min = heap.extractMin();
        std::cout << "Extraido: " << min << std::endl;
    }
    std::cout << "\nHeap apos todas as extracoes:" << std::endl;
    printHeapStatus(heap);

    // 4. Teste de exceção de estouro (overflow)
    std::cout << "\n--- Teste de Excecao de Overflow ---" << std::endl;
    try {
        std::cout << "Enchendo o heap ate a capacidade maxima..." << std::endl;
        for (int i = 0; i < 11; ++i) {
            heap.insert(i * 5);
            std::cout << "Inserido: " << i * 5 << ". Tamanho: " << heap.getSize() << std::endl;
        }
    } catch (const std::overflow_error& e) {
        std::cerr << "ERRO CAPTURADO (esperado): " << e.what() << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;

    // 5. Teste de exceção de esvaziamento (underflow)
    std::cout << "\n--- Teste de Excecao de Underflow ---" << std::endl;
    std::cout << "Esvaziando o heap..." << std::endl;
    while (!heap.isEmpty()) {
        heap.extractMin();
    }
    std::cout << "Heap esvaziado." << std::endl;
    
    try {
        std::cout << "Tentando extrair de um heap vazio..." << std::endl;
        heap.extractMin();
    } catch (const std::out_of_range& e) {
        std::cerr << "ERRO CAPTURADO (esperado): " << e.what() << std::endl;
    }

    std::cout << "\n--- Testes Concluidos ---" << std::endl;

    return 0;
}
