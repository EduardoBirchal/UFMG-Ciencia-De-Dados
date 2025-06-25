#include "escalonador.hpp"
#include "rede.hpp"
#include <stdexcept> 
#include <typeinfo>  
#include <iostream>

// Construtor
Escalonador::Escalonador(int maxEventos, int numPacotes, int latencia, int intervalo, Rede* r) 
    : filaDeEventos(maxEventos),
      rede(r),
      numPacotes(numPacotes),
      latenciaTransporte(latencia),
      intervaloTransportes(intervalo),
      horaAtual(0) {
    if (!rede) {
        throw std::invalid_argument("Ponteiro de rede nao pode ser nulo");
    }
    // Define o ponteiro do escalonador na rede para quebrar a dependência circular
    this->rede->setEscalonador(this);
}

// Destrutor
Escalonador::~Escalonador() {
    while (!filaDeEventos.isEmpty()) {
        EventoPtr eventoPtr = filaDeEventos.extractMin();
        delete eventoPtr.ptr;
    }
}

// Retorna ponteiro para a rede
Rede* Escalonador::getRede() const {
    return this->rede;
}

// Calcula a chave de prioridade para um evento
unsigned long long int Escalonador::calcularChave(const Evento& evento) const {
    if (evento.horaAgendada > 999999 || evento.horaAgendada < 0) {
        throw std::overflow_error("Hora fora do intervalo (0-999999)");
    }
    
    if (evento.valorTipo == 2) {
        int idPacote;
        
        if (evento.pacote)
            idPacote = evento.pacote->getIDEntrada();
        else
            idPacote = 0;
        
        if (idPacote > 999999 || idPacote < 0) {
            throw std::overflow_error("ID do pacote fora do intervalo (0-999999)");
        }

        unsigned long long int chaveCalculada = static_cast<unsigned long long int>(evento.horaAgendada) * 10000000ULL +
                                                static_cast<unsigned long long int>(idPacote) * 10ULL +
                                                2ULL;
        
        return chaveCalculada;
    } else {
        if (evento.idOrigem > 999 || evento.idOrigem < 0 || evento.idDestino > 999 || evento.idDestino < 0) {
            throw std::overflow_error("ID de origem ou destino fora do intervalo (0-999)");
        }

        unsigned long long int chaveCalculada = static_cast<unsigned long long int>(evento.horaAgendada) * 10000000ULL +
                                                static_cast<unsigned long long int>(evento.idOrigem) * 10000ULL +
                                                static_cast<unsigned long long int>(evento.idDestino) * 10ULL +
                                                1ULL;

        return chaveCalculada;
    }
}

// Retorna um ponteiro para o próximo evento da fila (sem remover)
Evento* Escalonador::proximo() const {
    if (filaDeEventos.isEmpty()) {
        throw std::out_of_range("Fila de eventos vazia");
    }
    return filaDeEventos.getMin().ptr;
}

// Remove e retorna um ponteiro para o próximo evento da fila
Evento* Escalonador::processarProximo() {
    if (filaDeEventos.isEmpty()) {
        throw std::out_of_range("Fila de eventos vazia");
    }
    Evento* evento = filaDeEventos.extractMin().ptr;
    this->horaAtual = evento->horaAgendada; // Atualiza a hora atual do sistema
    return evento;
}

// Processa, simula e deleta o próximo evento da fila
void Escalonador::simularProximo() {
    if (vazio()) return;

    Evento* evento = processarProximo(); // Pega o próximo evento
    evento->simular();                  // Executa sua lógica
    delete evento;                      // Libera a memória
}


// Verifica se a fila de eventos está vazia
bool Escalonador::vazio() const {
    return filaDeEventos.isEmpty();
}

// Retorna o número de eventos atualmente na fila
int Escalonador::tamanho() const {
    return filaDeEventos.getSize();
}

int Escalonador::getLatenciaTransporte() const {
    return this->latenciaTransporte;
}

int Escalonador::getIntervaloTransportes() const {
    return this->intervaloTransportes;
}
