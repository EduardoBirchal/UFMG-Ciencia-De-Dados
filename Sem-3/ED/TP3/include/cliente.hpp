#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include "linkedList.hpp"
#include "pacote.hpp"

class Pacote; // Declaração avançada para o ponteiro

/**
 * @class Cliente
 * @brief Representa um cliente no sistema de logística
 */
class Cliente {
private:
    std::string nome;
    linkedList<Pacote*> pacotesEnviar;
    linkedList<Pacote*> pacotesReceber;

public:
    /**
     * @brief Construtor que inicializa o cliente com um nome
     * @param nome O nome do cliente
     */
    Cliente(const std::string& nome = "");

    /**
     * @brief Destrutor
     */
    ~Cliente();

    bool operator<(const Cliente& outro) const;
    bool operator==(const Cliente& outro) const;

    /**
     * @brief Imprime informações sobre os pacotes do cliente.
     * Imprime o primeiro e o último evento de cada pacote enviado e recebido.
     */
    void printInformacao() const;

    // --- Getters ---
    std::string getNome() const;
    linkedList<Pacote*>& getPacotesEnviar();
    linkedList<Pacote*>& getPacotesReceber();

    // --- Setters ---
    void setNome(const std::string& nome);

    // --- Métodos de Manipulação ---
    void adicionarPacoteEnviar(Pacote* pacote);
    void adicionarPacoteReceber(Pacote* pacote);
};

#endif // CLIENTE_HPP
