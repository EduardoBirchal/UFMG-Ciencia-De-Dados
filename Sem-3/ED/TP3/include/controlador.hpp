#ifndef CONTROLADOR_HPP
#define CONTROLADOR_HPP

#include "avlTree.hpp"
#include "pacote.hpp"
#include "cliente.hpp"
#include "evento.hpp"
#include <string>

/**
 * @class Controlador
 * @brief Gerencia as estruturas de dados e a lógica de eventos do sistema.
 */
class Controlador {
private:
    avlTree<Pacote*> pacotes;
    avlTree<Cliente*> clientes;

public:
    /**
     * @brief Construtor
     */
    Controlador();

    /**
     * @brief Destrutor
     */
    ~Controlador();

    /**
     * @brief Processa uma query (evento ou consulta) a partir de uma string.
     * @param query A string da query a ser executada.
     */
    void executarQuery(const std::string& query);

    /**
     * @brief Processa um evento, aplicando sua lógica ao estado do sistema.
     * @param evento O evento a ser executado.
     */
    void executarEvento(const Evento& evento);

    // Métodos de adição e busca
    void adicionarPacote(Pacote* pacote);
    Pacote* acharPacote(int id);
    void adicionarCliente(Cliente* cliente);
    Cliente* acharCliente(const std::string& nome);
    Pacote* getOrCreatePacote(int id);
    Cliente* getOrCreateCliente(const std::string& nome);
};

#endif // CONTROLADOR_HPP
