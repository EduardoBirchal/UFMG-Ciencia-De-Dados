#include <iostream>
#include "../include/stack.hpp" // Inclui a sua classe de pilha

void run_tests() {
    std::cout << "--- Iniciando Testes da Pilha ---" << std::endl;

    // Cria uma pilha de inteiros
    stack<int> s;

    // Teste 1: A pilha deve começar vazia
    std::cout << "\nTeste 1: Verificando se a pilha recem-criada esta vazia..." << std::endl;
    if (s.isEmpty()) {
        std::cout << "Resultado: OK! A pilha esta vazia." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! A pilha deveria estar vazia." << std::endl;
    }

    // Teste 2: Adicionando elementos com push()
    std::cout << "\nTeste 2: Adicionando os valores 10, 20, 30 na pilha..." << std::endl;
    s.push(10);
    s.push(20);
    s.push(30);
    std::cout << "A pilha nao deve mais estar vazia." << std::endl;
    if (!s.isEmpty()) {
        std::cout << "Resultado: OK! A pilha nao esta vazia." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! A pilha nao deveria estar vazia." << std::endl;
    }

    // Teste 3: Verificando o topo da pilha com peek()
    std::cout << "\nTeste 3: Verificando o topo da pilha com peek()..." << std::endl;
    int topo = s.peek();
    std::cout << "Valor esperado no topo: 30" << std::endl;
    std::cout << "Valor retornado por peek(): " << topo << std::endl;
    if (topo == 30) {
        std::cout << "Resultado: OK!" << std::endl;
    } else {
        std::cout << "Resultado: FALHOU!" << std::endl;
    }

    // Teste 4: Removendo elementos com pop()
    std::cout << "\nTeste 4: Removendo elementos com pop() (ordem LIFO)..." << std::endl;
    std::cout << "Removendo -> " << s.pop() << " (Esperado: 30)" << std::endl;
    std::cout << "Removendo -> " << s.pop() << " (Esperado: 20)" << std::endl;
    std::cout << "Removendo -> " << s.pop() << " (Esperado: 10)" << std::endl;

    // Teste 5: A pilha deve estar vazia novamente
    std::cout << "\nTeste 5: Verificando se a pilha esta vazia apos os pops..." << std::endl;
    if (s.isEmpty()) {
        std::cout << "Resultado: OK! A pilha esta vazia." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! A pilha deveria estar vazia." << std::endl;
    }

    // Teste 6: Tentando usar pop() em uma pilha vazia (tratamento de exceção)
    std::cout << "\nTeste 6: Testando pop() em uma pilha vazia..." << std::endl;
    try {
        s.pop();
        std::cout << "Resultado: FALHOU! Uma excecao std::out_of_range deveria ter sido lancada." << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Resultado: OK! Capturou a excecao esperada." << std::endl;
        std::cout << "Mensagem da excecao: " << e.what() << std::endl;
    }

    std::cout << "\n--- Testes Concluidos ---" << std::endl;
}

int main() {
    run_tests();
    return 0;
}
