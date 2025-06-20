#include <iostream>
#include <string>      // Incluído para testar com strings
#include <stdexcept>   // Incluído para capturar exceções
#include "../include/linkedList.hpp" // Incluído o header correto

// Função auxiliar para imprimir e esvaziar a lista
// Isso demonstra as funções readHead() e remove()
template<class T>
void printAndClearList(linkedList<T>& list) {
    std::cout << "Conteúdo da lista: ";
    try {
        while (true) {
            std::cout << list.readHead() << " ";
            list.remove(0); // Remove o primeiro elemento
        }
    } catch (const std::out_of_range& e) {
        // A exceção é esperada quando a lista fica vazia
        std::cout << "(vazia)" << std::endl;
    }
}

void testIntegerList() {
    std::cout << "--- Testando linkedList com inteiros ---" << std::endl;
    
    // Cria uma instância da linkedList para inteiros
    linkedList<int> myList;
    
    // Adiciona alguns elementos
    myList.add(10);
    myList.add(20);
    myList.add(30);
    myList.add(40);
    myList.add(50);

    std::cout << "Lista inicial criada." << std::endl;

    // Remove o nó no índice 2 (valor 30)
    std::cout << "\nRemovendo o nó no índice 2..." << std::endl;
    myList.remove(2);
    
    // Remove o nó no índice 0 (a cabeça, valor 10)
    std::cout << "Removendo o nó no índice 0..." << std::endl;
    myList.remove(0);
    
    // Tenta remover em um índice inválido
    std::cout << "Tentando remover no índice 15 (inválido)..." << std::endl;
    bool success = myList.remove(15);
    if (!success) {
        std::cout << "Falha ao remover no índice 15, como esperado." << std::endl;
    }

    printAndClearList(myList);
}

void testStringList() {
    std::cout << "\n--- Testando linkedList com strings ---" << std::endl;
    
    // Cria uma instância da linkedList para strings
    linkedList<std::string> myStringList;
    
    // Adiciona alguns elementos
    myStringList.add("Olá");
    myStringList.add("Mundo");
    myStringList.add("!");

    std::cout << "Lista de strings criada." << std::endl;
    
    std::cout << "\nRemovendo o nó no índice 1 ('Mundo')..." << std::endl;
    myStringList.remove(1);

    printAndClearList(myStringList);
}


int main() {
    testIntegerList();
    testStringList();

    return 0;
}
