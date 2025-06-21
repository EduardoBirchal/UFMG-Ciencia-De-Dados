#include "pacote.hpp"

// Construtor
Pacote::Pacote(int id_pacote, int hora_postagem, int id_origem, int id_destino) : 
    id(id_pacote), 
    horaPostagem(hora_postagem),
    idOrigem(id_origem),
    idDestino(id_destino),
    idAtual(id_origem),      // Localização atual inicia na origem
    idSecaoAtual(0),         // Seção inicial padrão é 0
    estado(EstadoPacote::inicial) {
    // Construtor inicializa via lista
}

// Destrutor
Pacote::~Pacote() {
    // Nada a fazer
}

// Retorna o ID
int Pacote::getID() const {
    return this->id;
}

// Retorna a hora da postagem
int Pacote::getHoraPostagem() const {
    return this->horaPostagem;
}

// Retorna o ID de origem
int Pacote::getOrigem() const {
    return this->idOrigem;
}

// Retorna o ID de destino
int Pacote::getDestino() const {
    return this->idDestino;
}

// Retorna o ID da localização atual
int Pacote::getAtual() const {
    return this->idAtual;
}

// Retorna o ID da seção atual
int Pacote::getSecaoAtual() const {
    return this->idSecaoAtual;
}

// Retorna o estado atual
EstadoPacote Pacote::getEstado() const {
    return this->estado;
}

// Define o novo estado
void Pacote::setEstado(EstadoPacote novo_estado) {
    this->estado = novo_estado;
}

// Define a localização atual
void Pacote::setAtual(int id_atual) {
    this->idAtual = id_atual;
}

// Define a seção atual
void Pacote::setSecaoAtual(int id_secao_atual) {
    this->idSecaoAtual = id_secao_atual;
}
