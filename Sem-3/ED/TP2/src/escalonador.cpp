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
// Precisa liberar a memória de todos os eventos criados com 'new'
Escalonador::~Escalonador() {
    while (!filaDeEventos.isEmpty()) {
        EventoPtr eventoPtr = filaDeEventos.extractMin();
        delete eventoPtr.ptr; // Libera a memória do evento
    }
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

// A implementação de 'agendar' foi movida para o .hpp por ser um template

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
    // O chamador deste método se torna responsável por deletar o ponteiro retornado
    return filaDeEventos.extractMin().ptr;
}

// Verifica se a fila de eventos está vazia
bool Escalonador::vazio() const {
    return filaDeEventos.isEmpty();
}

// Retorna o número de eventos atualmente na fila
int Escalonador::tamanho() const {
    return filaDeEventos.getSize();
}
