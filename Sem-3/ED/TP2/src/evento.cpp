#include "evento.hpp"
#include "escalonador.hpp"
#include "armazem.hpp"
#include "estatisticas.hpp"

Evento::Evento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : chave(c), pacote(p), idOrigem(origem), idDestino(destino), idSecao(secao), horaAgendada(hora), escalonador(esc) {}
void Evento::setChave(unsigned long long int novaChave) { this->chave = novaChave; }
bool Evento::operator<(const Evento& outro) const { return this->chave < outro.chave; }
void Evento::simular() { /* Comportamento padrão não faz nada */ }
void Evento::imprimir() {}
void Evento::printHora(unsigned long long int hora) { std::cout << std::setw(7) << std::setfill('0') << hora; }
void Evento::printID(int id) { std::cout << std::setw(3) << std::setfill('0') << id; }

// --- EventoPostagem ---
EventoPostagem::EventoPostagem(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 1; }
void EventoPostagem::imprimir() { printHora(horaAgendada); std::cout << " pacote "; printID(pacote->getID()); std::cout << " armazenado em "; printID(escalonador->getRede()->getArmazem(idOrigem)->getID()); std::cout << " na secao "; printID(pacote->getDestinoAtual()); std::cout << "\n"; }
void EventoPostagem::simular() {
    if (!escalonador || !pacote) return;

    // A postagem é o início do ciclo de vida, contada aqui.
    Estatisticas::instancia().incrementarPostagens();

    Rede* rede = escalonador->getRede();
    if (!rede) return;
    if (!escalonador->foiPrimeiroTransporteAgendado()) {
        int proximaHora = this->horaAgendada + escalonador->getIntervaloTransportes();
        escalonador->agendar<EventoAcionarTransporte>(nullptr, 0, 0, 0, proximaHora);
        escalonador->marcarPrimeiroTransporteAgendado();
    }
    rede->calcularRota(pacote);
    if (pacote->getOrigem() == pacote->getDestinoFinal()) {
        escalonador->agendar<EventoEntrega>(pacote, idOrigem, idDestino, 0, horaAgendada);
        return;
    }
    Armazem* armazemOrigem = rede->getArmazem(this->idOrigem);
    if (armazemOrigem) {
        imprimir();
        armazemOrigem->adicionarPacote(this->pacote, this->pacote->getDestinoAtual());
    }
}

// --- EventoArmazenamento ---
// O contador foi movido para Armazem::adicionarPacote
EventoArmazenamento::EventoArmazenamento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }
void EventoArmazenamento::imprimir() { printHora(horaAgendada); std::cout << " pacote "; printID(pacote->getID()); std::cout << " armazenado em "; printID(idDestino); std::cout << " na secao "; printID(pacote->getDestinoAtual()); std::cout << "\n"; }
void EventoArmazenamento::simular() {
    if (!escalonador || !pacote) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;
    if (this->idDestino == this->pacote->getDestinoFinal()) {
        escalonador->agendar<EventoEntrega>(pacote, idOrigem, idDestino, 0, horaAgendada);
        return;
    }
    Armazem* armazemAtual = rede->getArmazem(this->idDestino);
    if (armazemAtual) {
        pacote->atualizarDestinoAtual();
        imprimir();
        armazemAtual->adicionarPacote(this->pacote, pacote->getDestinoAtual());
    }
}

// --- EventoEnviarPacote ---
// O contador foi movido para Armazem::encaminhaPilhaAuxiliar
EventoEnviarPacote::EventoEnviarPacote(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 1; }
void EventoEnviarPacote::imprimir() { printHora(horaAgendada); std::cout << " pacote "; printID(pacote->getID()); std::cout << " em transito de "; printID(idOrigem); std::cout << " para "; printID(idDestino); std::cout << "\n"; }
void EventoEnviarPacote::simular() {
    if (!escalonador || !pacote) return;
    imprimir();
    int horaChegada = horaAgendada + escalonador->getLatenciaTransporte();
    escalonador->agendar<EventoArmazenamento>(pacote, this->idOrigem, this->idDestino, 0, horaChegada);
}

// --- EventoRearmazenamento ---
// O contador foi movido para Armazem::encaminhaPilhaAuxiliar
EventoRearmazenamento::EventoRearmazenamento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 1; }
void EventoRearmazenamento::imprimir() { printHora(horaAgendada); std::cout << " pacote "; printID(pacote->getID()); std::cout << " rearmazenado em "; printID(idOrigem); std::cout << " na secao "; printID(idSecao); std::cout << "\n"; }
void EventoRearmazenamento::simular() {
    if (!pacote) return;
    imprimir();
}

// --- EventoEntrega ---
EventoEntrega::EventoEntrega(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }
void EventoEntrega::imprimir() { printHora(horaAgendada); std::cout << " pacote "; printID(pacote->getID()); std::cout << " entregue em "; printID(idDestino); std::cout << "\n"; }
void EventoEntrega::simular() { 
    if (!pacote) return;
    
    // A entrega é um evento final, o contador fica aqui.
    Estatisticas::instancia().incrementarEntregues();

    imprimir(); 
    delete this->pacote; 
    this->pacote = nullptr; 
}

// ... (Restante do arquivo sem alterações) ...
EventoAcionarTransporte::EventoAcionarTransporte(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }
void EventoAcionarTransporte::imprimir() {}
void EventoAcionarTransporte::simular() {
    if (!escalonador) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;

    int numArmazensRede = rede->getNumArmazens();
    Armazem** armazensParaProcessar = new Armazem*[numArmazensRede];
    int count = 0;
    for (int i = 0; i < numArmazensRede; ++i) {
        Armazem* armazem = rede->getArmazemPeloIndice(i);
        if (armazem && !armazem->secoesVazias()) {
            armazensParaProcessar[count++] = armazem;
        }
    }

    if (count > 0) {
        for (int i = 1; i < count; i++) {
            Armazem* chave = armazensParaProcessar[i];
            int j = i - 1;
            while (j >= 0 && armazensParaProcessar[j]->getID() > chave->getID()) {
                armazensParaProcessar[j + 1] = armazensParaProcessar[j];
                j = j - 1;
            }
            armazensParaProcessar[j + 1] = chave;
        }

        for (int i = 0; i < count; i++) {
            Armazem* armazem = armazensParaProcessar[i];
            escalonador->agendar<EventoAcionarArmazem>(nullptr, 0, armazem->getID(), 0, horaAgendada, Prioridade::ARMAZEM);
        }

        int proximaHora = this->horaAgendada + escalonador->getIntervaloTransportes();
        escalonador->agendar<EventoAcionarTransporte>(nullptr, 0, 0, 0, proximaHora, Prioridade::NORMAL);
    }
    
    delete[] armazensParaProcessar;
}

EventoAcionarArmazem::EventoAcionarArmazem(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }
void EventoAcionarArmazem::imprimir() {}
void EventoAcionarArmazem::simular() {
    if (!escalonador) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;
    
    Armazem* armazemAlvo = rede->getArmazem(this->idDestino);
    if (armazemAlvo) {
        int primeiraSecao = armazemAlvo->findProximaSecaoNaoVazia(-1);
        if (primeiraSecao != -1) {
            escalonador->agendar<EventoAcionarSecao>(nullptr, this->idDestino, 0, primeiraSecao, horaAgendada, Prioridade::SECAO);
        }
    }
}

EventoAcionarSecao::EventoAcionarSecao(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : Evento(c, p, origem, destino, secao, hora, esc) { valorTipo = 2; }
void EventoAcionarSecao::imprimir() {
    printHora(horaAgendada);
    std::cout << " acionando processamento do armazem ";
    printID(idOrigem);
    std::cout << " secao ";
    printID(idSecao);
    std::cout << "\n";
}
void EventoAcionarSecao::simular() {
    if (!escalonador) return;
    Rede* rede = escalonador->getRede();
    if (!rede) return;

    Armazem* armazemAlvo = rede->getArmazem(this->idOrigem);
    if (armazemAlvo) {
        armazemAlvo->processarEncaminhamentoSecao(this->idSecao, this->horaAgendada);
        int proximaSecao = armazemAlvo->findProximaSecaoNaoVazia(this->idSecao);
        if (proximaSecao != -1) {
            escalonador->agendar<EventoAcionarSecao>(nullptr, this->idOrigem, 0, proximaSecao, this->horaAgendada, Prioridade::SECAO);
        }
    }
}
