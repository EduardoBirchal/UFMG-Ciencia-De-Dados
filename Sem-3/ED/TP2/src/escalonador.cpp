#include "escalonador.hpp"
#include <stdexcept> // Para std::invalid_argument e std::overflow_error

// Construtor
Escalonador::Escalonador(int maxEventos, int numPacotes, int latencia, int intervalo) 
    : filaDeEventos(maxEventos),
      numPacotes(numPacotes),
      latenciaTransporte(latencia),
      intervaloTransportes(intervalo) {
    // Corpo vazio, inicialização feita na lista de inicializadores
}

// Destrutor
Escalonador::~Escalonador() {
    // Corpo vazio, MinHeap gerencia sua própria memória
}

// Calcula a chave de prioridade para um evento
unsigned long long int Escalonador::calcularChave(const Evento& evento) const {
    if (evento.horaAgendada > 999999 || evento.horaAgendada < 0) {
        throw std::overflow_error("Hora fora do intervalo (0-999999)");
    }
    
    if (evento.tipo == Transporte) {
        // [Hora (6)][ID Pacote (6)][Prioridade 2 (1)]
        int idPacote = evento.pacote->getID();
        if (idPacote > 999999 || idPacote < 0) {
            throw std::overflow_error("ID do pacote fora do intervalo (0-999999)");
        }
        return static_cast<unsigned long long int>(evento.horaAgendada) * 10000000ULL +
               static_cast<unsigned long long int>(idPacote) * 10ULL +
               2ULL;
    } else {
        // [Hora (6)][ID Origem (3)][ID Destino (3)][Prioridade 1 (1)]
        if (evento.idOrigem > 999 || evento.idOrigem < 0 || evento.idDestino > 999 || evento.idDestino < 0) {
             throw std::overflow_error("ID de origem ou destino fora do intervalo (0-999)");
        }
        return static_cast<unsigned long long int>(evento.horaAgendada) * 10000000ULL +
               static_cast<unsigned long long int>(evento.idOrigem) * 10000ULL +
               static_cast<unsigned long long int>(evento.idDestino) * 10ULL +
               1ULL;
    }
}

// Agenda um novo evento
void Escalonador::agendar(TipoEvento tipo, Pacote* pacote, int idOrigem, int idDestino, int idSecao, int horaAgendada) {
    if (!pacote) {
        throw std::invalid_argument("Ponteiro de pacote nao pode ser nulo ao agendar");
    }
    
    // Cria um evento com uma chave temporária (0)
    Evento novoEvento(0, tipo, pacote, idOrigem, idDestino, idSecao, horaAgendada);
    
    // Calcula a chave correta e a atribui ao evento
    novoEvento.chave = this->calcularChave(novoEvento);
    
    // Insere o evento com a chave correta na fila
    filaDeEventos.insert(novoEvento);
}

// Retorna o próximo evento da fila (sem remover)
Evento Escalonador::proximo() const {
    return filaDeEventos.getMin();
}

// Remove e retorna o próximo evento da fila
Evento Escalonador::processarProximo() {
    return filaDeEventos.extractMin();
}

// Verifica se a fila de eventos está vazia
bool Escalonador::vazio() const {
    return filaDeEventos.isEmpty();
}

// Retorna o número de eventos atualmente na fila
int Escalonador::tamanho() const {
    return filaDeEventos.getSize();
}
