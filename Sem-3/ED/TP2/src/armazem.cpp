#include "armazem.hpp"
#include "escalonador.hpp" 
#include "evento.hpp"      
#include <stdexcept>

// ... (Construtor e outras funções não mudam) ...
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
Armazem::~Armazem() { }
// findOrCreateSecao
Secao* Armazem::findOrCreateSecao(int id_secao) { Secao* secao = findSecao(id_secao); if (secao) { return secao; } Secao* nova_secao = new Secao(id_secao); secoes.addToTail(nova_secao); return nova_secao; }
// findSecao
Secao* Armazem::findSecao(int id_secao) const { for (int i = 0; i < secoes.getSize(); ++i) { Secao* secao_atual = secoes.readIndex(i); if (secao_atual->id == id_secao) { return secao_atual; } } return nullptr; }
// addSecao
void Armazem::addSecao(int id_secao) { findOrCreateSecao(id_secao); }
// adicionarPacote
void Armazem::adicionarPacote(Pacote* pacote, int id_secao_alvo) { pacote->setArmazemAtual(this->id); pacote->setEstado(EstadoPacote::armazenado); Secao* secao = findOrCreateSecao(id_secao_alvo); pacote->setSecaoAtual(id_secao_alvo); secao->pilha.push(pacote); }
// removerPacote
Pacote* Armazem::removerPacote(int id_secao) { Secao* secao = findSecao(id_secao); if (secao) { if (secao->pilha.isEmpty()) { return nullptr; } return secao->pilha.pop(); } throw std::out_of_range("ID de secao nao encontrado neste armazem"); }
// realocarSecao
int Armazem::realocarSecao(int id_secao, int horaInicial) { Secao* secao = findSecao(id_secao); if (!secao) return horaInicial; int i = 1; while(!secao->pilha.isEmpty()) { Pacote* pacote = secao->pilha.pop(); this->pilhaAuxiliar.push(pacote); int horaAgendada = horaInicial + (i * this->custoRemocao); Evento::printHora(horaAgendada); std::cout << " pacote "; Evento::printID(pacote->getID()); std::cout << " removido de "; Evento::printID(this->getID()); std::cout << " na secao "; Evento::printID(id_secao); std::cout << "\n"; i++; } return horaInicial + ((i-1) * this->custoRemocao); }


// MUDANÇA: Agendamento com PRIORIDADE ALTA para garantir execução imediata
void Armazem::encaminhaPilhaAuxiliar(int horaAtual) {
    if (!escalonador) return;
    
    Rede* rede = escalonador->getRede();
    if (!rede) return;
    int capacidadeTransporte = rede->getMaxTransporte();

    // Loop 1: agenda envio dos pacotes que devem sair com PRIORIDADE ALTA
    for (int i = 0; i < capacidadeTransporte && !pilhaAuxiliar.isEmpty(); ++i) {
        Pacote* pacote = popAuxiliar(); 
        if(pacote){
            escalonador->agendar<EventoEnviarPacote>(pacote, this->id, pacote->getDestinoAtual(), 0, horaAtual, Prioridade::ALTA);
        }
    }

    // Loop 2: agenda rearmazenamento dos pacotes que ficam com PRIORIDADE ALTA
    while (!pilhaAuxiliar.isEmpty()) {
        Pacote* pacote = popAuxiliar(); 
        if (pacote) {
            escalonador->agendar<EventoRearmazenamento>(pacote, this->id, 0, pacote->getSecaoAtual(), horaAtual, Prioridade::ALTA);
        }
    }
}

void Armazem::encaminharPacotes(int horaAtual) {
    if (!escalonador) return;

    for (int i = 0; i < secoes.getSize(); ++i) {
        Secao* secao = secoes.readIndex(i);
        if (secao && !secao->pilha.isEmpty()) {
            int novaHora = realocarSecao(secao->id, horaAtual);
            encaminhaPilhaAuxiliar(novaHora);
        }
    }
}

// ... (restante do arquivo sem mudanças) ...
bool Armazem::secoesVazias() const { for (int i = 0; i < secoes.getSize(); ++i) { Secao* secao_atual = secoes.readIndex(i); if (secao_atual && !secao_atual->pilha.isEmpty()) { return false; } } return true; }
int Armazem::getID() const { return this->id; }
int Armazem::getNumSecoes() const { return secoes.getSize(); }
Pacote* Armazem::popAuxiliar() { if (pilhaAuxiliar.isEmpty()) return nullptr; return pilhaAuxiliar.pop(); }
void Armazem::transfereParaAuxiliar(int id_secao) { Pacote* pacote = removerPacote(id_secao); if (pacote) { this->pilhaAuxiliar.push(pacote); } }
template class stack<Pacote*>;
template class linkedList<Secao*>;