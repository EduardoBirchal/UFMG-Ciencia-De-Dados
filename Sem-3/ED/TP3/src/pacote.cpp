#include "pacote.hpp"
#include <iostream> // Para usar std::cout

/**
 * @brief Construtor da classe Pacote
 * @param id O ID a ser atribuído ao pacote no momento da criação
 */
Pacote::Pacote(int id) : id(id) {
    // A lista 'historicoEventos' é inicializada automaticamente
}

/**
 * @brief Destrutor da classe Pacote
 */
Pacote::~Pacote() {
    // O destrutor de 'historicoEventos' será chamado automaticamente
}

bool Pacote::operator<(const Pacote& outro) const {
    return this->id < outro.id;
}

bool Pacote::operator==(const Pacote& outro) const {
    return this->id == outro.id;
}

/**
 * @brief Imprime as informações do pacote.
 */
void Pacote::printInformacao() const {
    int numEventos = this->historicoEventos.getSize();
    std::cout << numEventos << std::endl;

    for (int i = 0; i < numEventos; ++i) {
        // Usamos const_cast porque readIndex não é const, mas a lógica é segura.
        // O ideal seria ter uma versão const de readIndex.
        Evento& evento = const_cast<linkedList<Evento>&>(this->historicoEventos).readIndex(i);
        std::cout << evento.eventoPraString() << std::endl;
    }
}

// --- Implementação dos Getters ---

int Pacote::getId() const {
    return this->id;
}

Evento& Pacote::getPrimeiroEvento() {
    return this->historicoEventos.readHead();
}

Evento& Pacote::getUltimoEvento() {
    return this->historicoEventos.readTail();
}

linkedList<Evento>& Pacote::getHistoricoEventos() {
    return this->historicoEventos;
}

// --- Implementação dos Setters ---

void Pacote::setId(int id) {
    this->id = id;
}

// --- Outros Métodos ---

void Pacote::adicionarEvento(const Evento& evento) {
    this->historicoEventos.addToTail(evento);
}
