#include "armazem.hpp"
#include "escalonador.hpp"
#include "evento.hpp"      
#include <stdexcept>

// ... (Construtor, Destrutor, find... e métodos de adicionar/remover pacotes permanecem iguais)
Armazem::Armazem(int id_armazem, int custo_remocao, Escalonador* esc) : id(id_armazem), custoRemocao(custo_remocao), escalonador(esc), secoes(true), pilhaAuxiliar(false) { if (!escalonador) throw std::invalid_argument("Ponteiro do escalonador nao pode ser nulo no Armazem"); }
Armazem::~Armazem() { }
// CORREÇÃO: Alterado de addToTail para addOrdered
Secao* Armazem::findOrCreateSecao(int id_secao) { 
    Secao* secao = findSecao(id_secao); 
    if (secao) return secao; 
    
    Secao* nova_secao = new Secao(id_secao); 
    secoes.addOrdered(nova_secao); // <-- AQUI ESTÁ A CORREÇÃO
    return nova_secao; 
}
Secao* Armazem::findSecao(int id_secao) const { for (int i = 0; i < secoes.getSize(); ++i) { Secao* secao_atual = secoes.readIndex(i); if (secao_atual->id == id_secao) return secao_atual; } return nullptr; }
void Armazem::addSecao(int id_secao) { findOrCreateSecao(id_secao); }
void Armazem::adicionarPacote(Pacote* pacote, int id_secao_alvo) { pacote->setArmazemAtual(this->id); pacote->setEstado(EstadoPacote::armazenado); Secao* secao = findOrCreateSecao(id_secao_alvo); pacote->setSecaoAtual(id_secao_alvo); secao->pilha.push(pacote); }
Pacote* Armazem::removerPacote(int id_secao) { Secao* secao = findSecao(id_secao); if (secao) { if (secao->pilha.isEmpty()) return nullptr; return secao->pilha.pop(); } throw std::out_of_range("ID de secao nao encontrado neste armazem"); }


int Armazem::realocarSecao(int id_secao, int horaInicial) {
    Secao* secao = findSecao(id_secao);
    if (!secao || secao->pilha.isEmpty()) return horaInicial;
    
    int i = 1;
    while(!secao->pilha.isEmpty()) {
        Pacote* pacote = secao->pilha.pop();
        pilhaAuxiliar.push(pacote);
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
    return horaInicial + ((i-1) * this->custoRemocao);
}

void Armazem::encaminhaPilhaAuxiliar(int horaAtual) {
    if (!escalonador) return;
    
    Rede* rede = escalonador->getRede();
    if (!rede) return;
    int capacidadeTransporte = rede->getMaxTransporte();

    for (int i = 0; i < capacidadeTransporte && !pilhaAuxiliar.isEmpty(); ++i) {
        Pacote* pacote = pilhaAuxiliar.pop(); 
        if(pacote){
            escalonador->agendar<EventoEnviarPacote>(pacote, this->id, pacote->getDestinoAtual(), 0, horaAtual, Prioridade::ALTA);
        }
    }

    while (!pilhaAuxiliar.isEmpty()) {
        Pacote* pacote = pilhaAuxiliar.pop(); 
        if (pacote) {
            escalonador->agendar<EventoRearmazenamento>(pacote, this->id, 0, pacote->getSecaoAtual(), horaAtual, Prioridade::ALTA);
        }
    }
}

// MUDANÇA: Lógica de `encaminharPacotes` agora está aqui e processa uma única seção
void Armazem::processarEncaminhamentoSecao(int id_secao, int horaAtual) {
    if (!escalonador) return;
    Secao* secao = findSecao(id_secao);
    if (secao && !secao->pilha.isEmpty()) {
        int novaHora = realocarSecao(secao->id, horaAtual);
        encaminhaPilhaAuxiliar(novaHora);
    }
}

// MUDANÇA: Novo método para encontrar a próxima seção a ser processada
int Armazem::findProximaSecaoNaoVazia(int idSecaoAnterior) const {
    bool encontrouAnterior = (idSecaoAnterior == -1);
    for (int i = 0; i < secoes.getSize(); ++i) {
        Secao* secao_atual = secoes.readIndex(i);
        if (secao_atual) {
            if (encontrouAnterior && !secao_atual->pilha.isEmpty()) {
                return secao_atual->id; // Retorna o ID da primeira seção não-vazia encontrada
            }
            if (secao_atual->id == idSecaoAnterior) {
                encontrouAnterior = true;
            }
        }
    }
    return -1; // Nenhuma outra seção não-vazia encontrada
}

// NOVO: Imprime os IDs de todas as seções deste armazém
void Armazem::imprimirSecoes() const {
    std::cout << "Secoes do Armazem " << this->id << ": ";
    if (secoes.isEmpty()) {
        std::cout << "[vazio]";
    } else {
        for (int i = 0; i < secoes.getSize(); ++i) {
            Secao* secao = secoes.readIndex(i);
            if (secao) {
                std::cout << secao->id << " ";
            }
        }
    }
    std::cout << std::endl;
}

bool Armazem::secoesVazias() const { for (int i = 0; i < secoes.getSize(); ++i) { Secao* secao_atual = secoes.readIndex(i); if (secao_atual && !secao_atual->pilha.isEmpty()) return false; } return true; }
int Armazem::getID() const { return this->id; }
int Armazem::getNumSecoes() const { return secoes.getSize(); }