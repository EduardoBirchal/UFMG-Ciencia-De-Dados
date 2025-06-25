#include "armazem.hpp"
#include "escalonador.hpp" // Para poder chamar o método agendar()
#include "evento.hpp"      // Para poder agendar eventos
#include <stdexcept>

// Construtor
Armazem::Armazem(int id_armazem, int custo_remocao, Escalonador* esc) 
    : id(id_armazem), 
      custoRemocao(custo_remocao),
      escalonador(esc),
      secoes(true), 
      pilhaAuxiliar(false) {
    if (!escalonador) {
        throw std::invalid_argument("Ponteiro do escalonador nao pode ser nulo no Armazem");
    }
}

// Destrutor
Armazem::~Armazem() {
    // O destrutor da lista 'secoes' gerencia a liberação da memória
}

// Método auxiliar privado para encontrar ou criar uma seção
Secao* Armazem::findOrCreateSecao(int id_secao) {
    Secao* secao = findSecao(id_secao);
    if (secao) {
        return secao;
    }
    // Se não encontrou, cria uma nova seção
    Secao* nova_secao = new Secao(id_secao);
    secoes.addToTail(nova_secao);
    return nova_secao; // Retorna a seção recém-criada
}

// Método auxiliar privado para encontrar uma seção
Secao* Armazem::findSecao(int id_secao) const {
    for (int i = 0; i < secoes.getSize(); ++i) {
        Secao* secao_atual = secoes.readIndex(i);
        if (secao_atual->id == id_secao) {
            return secao_atual;
        }
    }
    return nullptr; // Não encontrou
}


// Adiciona uma nova seção com um ID específico
void Armazem::addSecao(int id_secao) {
    findOrCreateSecao(id_secao);
}


// Adiciona um pacote a sua seção designada
void Armazem::adicionarPacote(Pacote* pacote, int id_secao_alvo) {
    pacote->setArmazemAtual(this->id);
    pacote->setEstado(EstadoPacote::armazenado);
    // A atualização da rota agora é responsabilidade do evento
    Secao* secao = findOrCreateSecao(id_secao_alvo);
    pacote->setSecaoAtual(id_secao_alvo);
    secao->pilha.push(pacote);
}

// Remove um pacote do topo de uma seção
Pacote* Armazem::removerPacote(int id_secao) {
    Secao* secao = findSecao(id_secao);
    if (secao) {
        if (secao->pilha.isEmpty()) {
            return nullptr; 
        }
        return secao->pilha.pop();
    }
    throw std::out_of_range("ID de secao nao encontrado neste armazem");
}

// Esvazia uma seção e agenda o anúncio de suas remoções
int Armazem::realocarSecao(int id_secao, int horaInicial) {
    Secao* secao = findSecao(id_secao);
    if (!secao) return horaInicial;

    int i = 1;
    while(!secao->pilha.isEmpty()) {
        Pacote* pacote = secao->pilha.pop();
        this->pilhaAuxiliar.push(pacote);
        int horaAgendada = horaInicial + (i * this->custoRemocao);

        Evento::printHora(horaAgendada);
        std::cout << " pacote ";
        Evento::printID(pacote->getID());
        std::cout << " removido de ";
        Evento::printID(this->getID());
        std::cout << " na secao ";
        Evento::printID(id_secao);
        std::cout << "\n";
        
        i++;
    }
    // Retorna a hora em que a realocação termina
    return horaInicial + ((i-1) * this->custoRemocao);
}

// Agenda o envio dos pacotes da pilha auxiliar
void Armazem::encaminhaPilhaAuxiliar(int horaAtual) {
    if (!escalonador) return;
    
    Rede* rede = escalonador->getRede();
    if (!rede) return;
    int capacidadeTransporte = rede->getMaxTransporte();

    // Loop 1: agenda envio dos pacotes que devem sair
    for (int i = 0; i < capacidadeTransporte && !pilhaAuxiliar.isEmpty(); ++i) {
        Pacote* pacote = popAuxiliar(); 
        if(pacote){
            escalonador->agendar<EventoEnviarPacote>(pacote, this->id, pacote->getDestinoAtual(), 0, horaAtual);
        }
    }

    // Loop 2: agenda rearmazenamento dos pacotes que ficam
    while (!pilhaAuxiliar.isEmpty()) {
        Pacote* pacote = popAuxiliar(); 
        if (pacote) {
            escalonador->agendar<EventoRearmazenamento>(pacote, this->id, 0, pacote->getSecaoAtual(), horaAtual);
            adicionarPacote(pacote, pacote->getSecaoAtual());
        }
    }
}

// Processa todas as seções não-vazias para encaminhamento
void Armazem::encaminharPacotes(int horaAtual) {
    if (!escalonador) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;

    for (int i = 0; i < secoes.getSize(); ++i) {
        Secao* secao = secoes.readIndex(i);
        if (secao && !secao->pilha.isEmpty()) {
            int novaHora = realocarSecao(secao->id, horaAtual);
            encaminhaPilhaAuxiliar(novaHora);
        }
    }
}

// Verifica se todas as seções do armazém estão vazias
bool Armazem::secoesVazias() const {
    for (int i = 0; i < secoes.getSize(); ++i) {
        Secao* secao_atual = secoes.readIndex(i);
        if (secao_atual && !secao_atual->pilha.isEmpty()) {
            return false; // Encontrou uma seção com pacotes, retorna falso
        }
    }
    return true; // Se o loop terminar, todas as seções estão vazias
}

// Getters
int Armazem::getID() const {
    return this->id;
}
int Armazem::getNumSecoes() const {
    return secoes.getSize();
}
Pacote* Armazem::popAuxiliar() {
    if (pilhaAuxiliar.isEmpty()) return nullptr;
    return pilhaAuxiliar.pop();
}
void Armazem::transfereParaAuxiliar(int id_secao) {
    Pacote* pacote = removerPacote(id_secao);
    if (pacote) {
        this->pilhaAuxiliar.push(pacote);
    }
}

// --- Instanciação Explícita de Template ---
template class stack<Pacote*>;
template class linkedList<Secao*>;
