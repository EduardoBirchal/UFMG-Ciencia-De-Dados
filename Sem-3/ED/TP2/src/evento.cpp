#include "evento.hpp"
#include "escalonador.hpp"
#include "armazem.hpp"

// --- Classe Base 'Evento' ---

Evento::Evento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : 
    chave(c), pacote(p), idOrigem(origem), idDestino(destino), idSecao(secao), horaAgendada(hora), escalonador(esc) {}

void Evento::setChave(unsigned long long int novaChave) { this->chave = novaChave; }
bool Evento::operator<(const Evento& outro) const { return this->chave < outro.chave; }
void Evento::simular() { /* Comportamento padrão não faz nada */ }
void Evento::imprimir() {}

void Evento::printHora(unsigned long long int hora) {
    std::cout << std::setw(7) << std::setfill('0') << hora;
}

void Evento::printID(int id) {
    std::cout << std::setw(3) << std::setfill('0') << id;
}

// --- EventoPostagem ---

EventoPostagem::EventoPostagem(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 1; }

void EventoPostagem::imprimir() {
    printHora(horaAgendada);
    std::cout << " pacote ";
    printID(pacote->getID());
    std::cout << " armazenado em ";
    printID(escalonador->getRede()->getArmazem(idOrigem)->getID());
    std::cout << " na secao ";
    printID(pacote->getDestinoAtual());
    std::cout << "\n";
}

void EventoPostagem::simular() {
    if (!escalonador || !pacote) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;

    rede->calcularRota(pacote);

    // Se o destino é local, agenda a entrega imediatamente
    if (pacote->getOrigem() == pacote->getDestinoFinal()) {
        escalonador->agendar<EventoEntrega>(pacote, idOrigem, idDestino, 0, horaAgendada);
        return;
    }
    
    // Senão, calcula a rota e armazena no primeiro armazém
    rede->calcularRota(this->pacote);
    Armazem* armazemOrigem = rede->getArmazem(this->idOrigem);
    if (armazemOrigem) {
        imprimir();
        armazemOrigem->adicionarPacote(this->pacote, this->pacote->getDestinoAtual());
    }
}

// --- EventoArmazenamento ---

EventoArmazenamento::EventoArmazenamento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }

void EventoArmazenamento::imprimir() {
    printHora(horaAgendada);
    std::cout << " pacote ";
    printID(pacote->getID());
    std::cout << " armazenado em ";
    printID(idDestino);
    std::cout << " na secao ";
    printID(pacote->getDestinoAtual());
    std::cout << "\n";
}

void EventoArmazenamento::simular() {
    if (!escalonador || !pacote) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;

    // Se a chegada é no destino final, agenda a entrega
    if (this->idDestino == this->pacote->getDestinoFinal()) {
        escalonador->agendar<EventoEntrega>(pacote, idOrigem, idDestino, 0, horaAgendada);
        return;
    }

    // Senão, armazena no armazém intermediário
    Armazem* armazemAtual = rede->getArmazem(this->idDestino);
    if (armazemAtual) {
        pacote->atualizarDestinoAtual(); // Descobre o próximo hop
        imprimir();
        armazemAtual->adicionarPacote(this->pacote, pacote->getDestinoAtual());
    }
}

// --- EventoEnviarPacote ---

EventoEnviarPacote::EventoEnviarPacote(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 1; }

void EventoEnviarPacote::imprimir() {
    printHora(horaAgendada);
    std::cout << " pacote ";
    printID(pacote->getID());
    std::cout << " em transito de ";
    printID(idOrigem);
    std::cout << " para ";
    printID(idDestino);
    std::cout << "\n";
}

void EventoEnviarPacote::simular() {
    if (!escalonador || !pacote) return;
    
    imprimir();

    int horaChegada = horaAgendada + escalonador->getLatenciaTransporte();
    // Agenda um evento de armazenamento para quando o pacote chegar no destino
    escalonador->agendar<EventoArmazenamento>(pacote, this->idOrigem, this->idDestino, 0, horaChegada);
}

// --- EventoRearmazenamento ---

EventoRearmazenamento::EventoRearmazenamento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 1; }

void EventoRearmazenamento::imprimir() {
    printHora(horaAgendada);
    std::cout << " pacote ";
    printID(pacote->getID());
    std::cout << " rearmazenado em ";
    printID(idOrigem);
    std::cout << " na secao ";
    printID(idSecao);
    std::cout << "\n";
}

void EventoRearmazenamento::simular() {
    if (!pacote) return;
    std::cout << this->horaAgendada << " pacote " << this->pacote->getID() << " rearmazenado em " << this->idOrigem << " na secao " << this->idSecao << "\n";
}

// --- EventoEntrega ---

EventoEntrega::EventoEntrega(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }

void EventoEntrega::imprimir() {
    printHora(horaAgendada);
    std::cout << " pacote ";
    printID(pacote->getID());
    std::cout << " entregue em ";
    printID(idDestino);
    std::cout << "\n";
}

void EventoEntrega::simular() {
    if (!pacote) return;
    imprimir();
    delete this->pacote;
    this->pacote = nullptr;
}

// --- EventoAcionarTransporte ---

EventoAcionarTransporte::EventoAcionarTransporte(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }

void EventoAcionarTransporte::imprimir() {}

void EventoAcionarTransporte::simular() {
    if (!escalonador) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;

    bool algumArmazemNaoVazio = false;
    // Itera sobre todos os armazéns da rede
    for (int i = 0; i < rede->getNumArmazens(); ++i) {
        Armazem* armazem = rede->getArmazemPeloIndice(i);
        if (armazem && !armazem->secoesVazias()) {
            algumArmazemNaoVazio = true; // Marca que a rede ainda tem pacotes
            armazem->encaminharPacotes(this->horaAgendada);
        }
    }
    
    // Se a rede não estiver completamente vazia, agenda o próximo ciclo de transporte
    if (algumArmazemNaoVazio) {
        int proximaHora = this->horaAgendada + escalonador->getIntervaloTransportes();
        // O evento agora é global, não precisa de um idOrigem específico
        escalonador->agendar<EventoAcionarTransporte>(nullptr, 0, 0, 0, proximaHora);
    }
}
