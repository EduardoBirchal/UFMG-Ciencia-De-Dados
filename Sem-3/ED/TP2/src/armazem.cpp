#include "armazem.hpp"
#include <stdexcept>

// Construtor
Armazem::Armazem(int id_armazem, int num_secoes) : id(id_armazem) {
    // Cria as seções (pilhas) vazias no armazém
    for (int i = 0; i < num_secoes; ++i) {
        stack<Pacote*> *nova_secao = new stack<Pacote*>(true);
        secoes.addToTail(nova_secao);
    }
}

// Destrutor
Armazem::~Armazem() {
    // A memória dos ponteiros de Pacote deve ser gerenciada externamente
}

// Adiciona um pacote a sua seção designada
void Armazem::adicionarPacote(Pacote* pacote) {
    int id_secao = pacote->getSecaoAtual();

    // Valida o ID da seção
    if (id_secao < 0 || id_secao >= secoes.getSize()) {
        throw std::out_of_range("ID de secao invalido para este armazem");
    }

    // Pega a referência para a pilha correta e adiciona o pacote
    secoes.readIndex(id_secao)->push(pacote);
    
    // Atualiza o estado e a localização do pacote
    pacote->setEstado(EstadoPacote::armazenado);
    pacote->setAtual(this->id);
}

// Remove um pacote do topo de uma seção
Pacote* Armazem::removerPacote(int id_secao) {
    // Valida o ID da seção
    if (id_secao < 0 || id_secao >= secoes.getSize()) {
        throw std::out_of_range("ID de secao invalido para este armazem");
    }

    // Pega a referência para a pilha e remove o pacote se não estiver vazia
    stack<Pacote*> *secao = secoes.readIndex(id_secao);
    if (secao->isEmpty()) {
        return nullptr; // Retorna nulo se a seção estiver vazia
    }

    return secao->pop();
}

// Retorna o ID do armazém
int Armazem::getID() const {
    return this->id;
}

// Retorna o número de seções no armazém
int Armazem::getNumSecoes() const {
    return secoes.getSize();
}

// --- Instanciação Explícita de Template ---
// Força o compilador a gerar o código para estas versões específicas dos templates,
// tornando-as disponíveis para o linker.
template class stack<Pacote*>;
template class linkedList<stack<Pacote*>*>;