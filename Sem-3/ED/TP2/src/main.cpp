#include <iostream>
#include "armazem.hpp"
#include "pacote.hpp"

// Função auxiliar para imprimir o estado de um pacote
void printPacoteInfo(const std::string& acao, Pacote* pacote) {
    if (pacote) {
        std::cout << acao << " - Pacote ID: " << pacote->getID() 
                  << " | Destino: " << pacote->getDestino()
                  << " | Estado: " << pacote->getEstado() 
                  << " | Localizacao: " << pacote->getAtual() 
                  << std::endl;
    } else {
        std::cout << acao << " - Nenhum pacote retornado (secao vazia)" << std::endl;
    }
}

void run_armazem_tests() {
    std::cout << "--- Iniciando Testes do Armazem ---" << std::endl;

    // Teste 1: Criação do Armazém
    std::cout << "\nTeste 1: Criando um armazem com ID 101 e 3 secoes..." << std::endl;
    Armazem armazem(101, 3);
    if (armazem.getID() == 101 && armazem.getNumSecoes() == 3) {
        std::cout << "Resultado: OK! Armazem criado com sucesso." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! Erro na criacao do armazem." << std::endl;
    }

    // Cria alguns pacotes para teste (alocados dinamicamente)
    Pacote* p1 = new Pacote(1, 10, 201, 301);
    Pacote* p2 = new Pacote(2, 11, 202, 302);
    Pacote* p3 = new Pacote(3, 12, 203, 303);
    
    // Define a seção de destino para cada pacote
    p1->setSecaoAtual(0);
    p2->setSecaoAtual(0); // p2 vai para a mesma seção de p1
    p3->setSecaoAtual(2);

    // Teste 2: Adicionar pacotes
    std::cout << "\nTeste 2: Adicionando pacotes ao armazem..." << std::endl;
    armazem.adicionarPacote(p1);
    printPacoteInfo("Adicionado", p1);
    armazem.adicionarPacote(p2);
    printPacoteInfo("Adicionado", p2);
    armazem.adicionarPacote(p3);
    printPacoteInfo("Adicionado", p3);

    if (p1->getAtual() == 101 && p1->getEstado() == EstadoPacote::armazenado &&
        p2->getAtual() == 101 && p2->getEstado() == EstadoPacote::armazenado &&
        p3->getAtual() == 101 && p3->getEstado() == EstadoPacote::armazenado) {
        std::cout << "Resultado: OK! Pacotes foram armazenados e seus estados atualizados." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! O estado ou a localizacao dos pacotes nao foi atualizada." << std::endl;
    }

    // Teste 3: Remover pacotes (LIFO)
    std::cout << "\nTeste 3: Removendo pacotes do armazem..." << std::endl;
    
    // Remove da seção 0, deve sair o p2 primeiro (último a entrar)
    Pacote* removido1 = armazem.removerPacote(0);
    printPacoteInfo("Removido da secao 0", removido1);
    if (removido1 && removido1->getID() == 2) {
        std::cout << "Resultado: OK! Pacote correto (ID 2) removido." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! Pacote incorreto ou nulo." << std::endl;
    }

    // Remove da seção 2
    Pacote* removido2 = armazem.removerPacote(2);
    printPacoteInfo("Removido da secao 2", removido2);
     if (removido2 && removido2->getID() == 3) {
        std::cout << "Resultado: OK! Pacote correto (ID 3) removido." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! Pacote incorreto ou nulo." << std::endl;
    }

    // Tenta remover da seção 1 (vazia)
    std::cout << "Tentando remover da secao 1 (vazia)..." << std::endl;
    Pacote* removido3 = armazem.removerPacote(1);
    printPacoteInfo("Removido da secao 1", removido3);
    if (removido3 == nullptr) {
        std::cout << "Resultado: OK! Nenhum pacote foi removido, como esperado." << std::endl;
    } else {
        std::cout << "Resultado: FALHOU! Um pacote foi retornado indevidamente." << std::endl;
    }

    // Teste 4: Tratamento de exceção
    std::cout << "\nTeste 4: Tentando adicionar pacote em secao invalida..." << std::endl;
    Pacote* p4 = new Pacote(4, 13, 204, 304);
    p4->setSecaoAtual(99); // Seção inválida
    try {
        armazem.adicionarPacote(p4);
        std::cout << "Resultado: FALHOU! Excecao std::out_of_range nao foi lancada." << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Resultado: OK! Capturou a excecao esperada." << std::endl;
        std::cout << "Mensagem: " << e.what() << std::endl;
    }

    // Limpeza da memória
    std::cout << "\n--- Limpando memoria ---" << std::endl;
    delete p1; // p1 ainda está na lista, mas o ponteiro ainda existe
    delete removido1; // p2
    delete removido2; // p3
    delete p4;
    std::cout << "Memoria liberada." << std::endl;

    std::cout << "\n--- Testes do Armazem Concluidos ---" << std::endl;
}

int main() {
    run_armazem_tests();
    return 0;
}
