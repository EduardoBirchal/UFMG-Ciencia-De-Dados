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
      horaAtual(0),
      primeiroTransporteAgendado(false) {
    if (!rede) {
        throw std::invalid_argument("Ponteiro de rede nao pode ser nulo");
    }
    this->rede->setEscalonador(this);
}

// Destrutor
Escalonador::~Escalonador() {
    while (!filaDeEventos.isEmpty()) {
        EventoPtr eventoPtr = filaDeEventos.extractMin();
        delete eventoPtr.ptr;
    }
}

// MUDANÇA: A função agora aceita prioridade e a insere na chave de ordenação.
// A chave agora é composta por: [HORA (7 dígitos)][PRIORIDADE (1 dígito)][DETALHES (8 dígitos)]
unsigned long long int Escalonador::calcularChave(const Evento& evento, Prioridade prioridade) const {
    if (evento.horaAgendada > 9999999 || evento.horaAgendada < 0) {
        throw std::overflow_error("Hora fora do intervalo (0-9999999)");
    }
    
    unsigned long long int parteHora = static_cast<unsigned long long int>(evento.horaAgendada) * 1000000000ULL;
    unsigned long long int partePrioridade = static_cast<unsigned long long int>(prioridade) * 100000000ULL;
    unsigned long long int parteDetalhes;

    if (evento.valorTipo == 2) { // Eventos de sistema/entrega
        int idPacote = evento.pacote ? evento.pacote->getIDEntrada() : 0;
        if (idPacote > 9999999) throw std::overflow_error("ID do pacote fora do intervalo");
        parteDetalhes = static_cast<unsigned long long int>(idPacote) * 10ULL + evento.valorTipo;
    } else { // Eventos de transporte
        if (evento.idOrigem > 9999 || evento.idDestino > 9999) throw std::overflow_error("ID de origem/destino fora do intervalo");
        parteDetalhes = static_cast<unsigned long long int>(evento.idOrigem) * 100000ULL +
                        static_cast<unsigned long long int>(evento.idDestino) * 10ULL +
                        evento.valorTipo;
    }

    return parteHora + partePrioridade + parteDetalhes;
}

// ... (restante do arquivo sem mudanças) ...
Rede* Escalonador::getRede() const { return this->rede; }
Evento* Escalonador::proximo() const { if (filaDeEventos.isEmpty()) { throw std::out_of_range("Fila de eventos vazia"); } return filaDeEventos.getMin().ptr; }
Evento* Escalonador::processarProximo() { if (filaDeEventos.isEmpty()) { throw std::out_of_range("Fila de eventos vazia"); } Evento* evento = filaDeEventos.extractMin().ptr; this->horaAtual = evento->horaAgendada; return evento; }
void Escalonador::simularProximo() { if (vazio()) return; Evento* evento = processarProximo(); evento->simular(); delete evento; }
bool Escalonador::vazio() const { return filaDeEventos.isEmpty(); }
int Escalonador::tamanho() const { return filaDeEventos.getSize(); }
int Escalonador::getLatenciaTransporte() const { return this->latenciaTransporte; }
int Escalonador::getIntervaloTransportes() const { return this->intervaloTransportes; }
bool Escalonador::foiPrimeiroTransporteAgendado() const { return this->primeiroTransporteAgendado; }
void Escalonador::marcarPrimeiroTransporteAgendado() { this->primeiroTransporteAgendado = true; }