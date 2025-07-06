#include "Cliente.hpp"
#include <iostream> // Para usar std::cout

/**
 * @brief Construtor
 * @param nome O nome para inicializar o cliente.
 * As listas não gerenciam a memória dos ponteiros.
 */
Cliente::Cliente(const std::string& nome) 
    : nome(nome), pacotesEnviar(false), pacotesReceber(false) {
}

/**
 * @brief Destrutor
 */
Cliente::~Cliente() {
}

bool Cliente::operator<(const Cliente& outro) const {
    return this->nome < outro.nome;
}

bool Cliente::operator==(const Cliente& outro) const {
    return this->nome == outro.nome;
}

/**
 * @brief Imprime informações sobre os pacotes do cliente.
 */
void Cliente::printInformacao() const {
    int tamanhoEnviar = this->pacotesEnviar.getSize();
    int tamanhoReceber = this->pacotesReceber.getSize();

    std::cout << (tamanhoEnviar + tamanhoReceber) * 2 << std::endl;

    // Imprime o primeiro evento de cada pacote enviado
    for (int i = 0; i < tamanhoEnviar; ++i) {
        Pacote* pacote = this->pacotesEnviar.readIndex(i);
        if (pacote && !pacote->getHistoricoEventos().isEmpty()) {
            std::cout << pacote->getPrimeiroEvento().eventoPraString() << std::endl;
        }
    }

    // Imprime o primeiro evento de cada pacote recebido
    for (int i = 0; i < tamanhoReceber; ++i) {
        Pacote* pacote = this->pacotesReceber.readIndex(i);
        if (pacote && !pacote->getHistoricoEventos().isEmpty()) {
            std::cout << pacote->getPrimeiroEvento().eventoPraString() << std::endl;
        }
    }

    // Imprime o último evento de cada pacote enviado
    for (int i = 0; i < tamanhoEnviar; ++i) {
        Pacote* pacote = this->pacotesEnviar.readIndex(i);
        if (pacote && !pacote->getHistoricoEventos().isEmpty()) {
            std::cout << pacote->getUltimoEvento().eventoPraString() << std::endl;
        }
    }

    // Imprime o último evento de cada pacote recebido
    for (int i = 0; i < tamanhoReceber; ++i) {
        Pacote* pacote = this->pacotesReceber.readIndex(i);
        if (pacote && !pacote->getHistoricoEventos().isEmpty()) {
            std::cout << pacote->getUltimoEvento().eventoPraString() << std::endl;
        }
    }
}

// --- Implementação dos Getters ---

std::string Cliente::getNome() const {
    return this->nome;
}

linkedList<Pacote*>& Cliente::getPacotesEnviar() {
    return this->pacotesEnviar;
}

linkedList<Pacote*>& Cliente::getPacotesReceber() {
    return this->pacotesReceber;
}

// --- Implementação dos Setters ---

void Cliente::setNome(const std::string& nome) {
    this->nome = nome;
}

// --- Implementação dos Métodos de Manipulação ---

void Cliente::adicionarPacoteEnviar(Pacote* pacote) {
    this->pacotesEnviar.addToTail(pacote);
}

void Cliente::adicionarPacoteReceber(Pacote* pacote) {
    this->pacotesReceber.addToTail(pacote);
}
