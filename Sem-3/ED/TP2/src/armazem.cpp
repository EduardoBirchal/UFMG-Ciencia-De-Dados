#include "armazem.hpp"
#include <stdexcept>

// Construtor: um armazém agora começa com 0 seções
Armazem::Armazem(int id_armazem) 
    : id(id_armazem), secoes(true) { // 'true' para que a lista delete os ponteiros de Secao
    // Nenhuma seção é criada inicialmente
}

// Destrutor
Armazem::~Armazem() {
    // O destrutor da lista 'secoes' gerencia a liberação da memória
}

// Método auxiliar privado para encontrar ou criar uma seção
Secao* Armazem::findOrCreateSecao(int id_secao) {
    // Procura por uma seção existente com o ID fornecido
    for (int i = 0; i < secoes.getSize(); ++i) {
        Secao* secao_atual = secoes.readIndex(i);
        if (secao_atual->id == id_secao) {
            return secao_atual; // Retorna a seção encontrada
        }
    }

    // Se não encontrou, cria uma nova seção
    Secao* nova_secao = new Secao(id_secao);
    secoes.addToTail(nova_secao);
    return nova_secao; // Retorna a seção recém-criada
}


// Adiciona uma nova seção com um ID específico
void Armazem::addSecao(int id_secao) {
    // Apenas chama o método auxiliar, que fará o trabalho
    findOrCreateSecao(id_secao);
}


// Adiciona um pacote a sua seção designada
void Armazem::adicionarPacote(Pacote* pacote) {
    // Atualiza a localização atual e o estado do pacote
    pacote->setArmazemAtual(this->id);
    pacote->setEstado(EstadoPacote::armazenado);

    // Faz o pacote determinar seu próximo destino na rota
    pacote->atualizarDestinoAtual();

    // Encontra a seção correspondente ao próximo destino ou cria uma nova
    int id_secao_alvo = pacote->getDestinoAtual();
    Secao* secao_alvo = findOrCreateSecao(id_secao_alvo);

    // Atualiza a seção atual do pacote e o armazena
    pacote->setSecaoAtual(id_secao_alvo);
    secao_alvo->pilha.push(pacote);
}

// Remove um pacote do topo de uma seção
Pacote* Armazem::removerPacote(int id_secao) {
    // Procura pela seção
    for (int i = 0; i < secoes.getSize(); ++i) {
        Secao* secao_atual = secoes.readIndex(i);
        if (secao_atual->id == id_secao) {
            if (secao_atual->pilha.isEmpty()) {
                return nullptr; // Seção encontrada, mas vazia
            }
            return secao_atual->pilha.pop();
        }
    }
    
    // Se a seção não for encontrada, lança uma exceção
    throw std::out_of_range("ID de secao nao encontrado neste armazem");
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
template class stack<Pacote*>;
template class linkedList<Secao*>;
