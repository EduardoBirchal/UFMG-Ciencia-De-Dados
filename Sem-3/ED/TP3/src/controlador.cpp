#include "controlador.hpp"
#include <sstream>   // Para usar std::stringstream
#include <iostream>  // Para usar std::cout
#include <iomanip>   // Para usar std::setw e std::setfill
#include "utils.hpp"

/**
 * @brief Construtor do Controlador.
 */
Controlador::Controlador() : pacotes(true), clientes(true) {
}

/**
 * @brief Destrutor do Controlador.
 */
Controlador::~Controlador() {
}

/**
 * @brief Processa uma query a partir de uma string.
 */
void Controlador::executarQuery(const std::string& query) {
    std::stringstream ss(query);
    int hora;
    std::string tipoQuery;

    // Lê a hora e o tipo da query
    ss >> hora >> tipoQuery;

    if (tipoQuery == "EV") {
        // Se for um evento, cria o objeto Evento a partir da linha inteira e o executa
        Evento evento = Evento::stringPraEvento(query);
        executarEvento(evento);

    } else if (tipoQuery == "CL") {
        // Se for uma consulta de cliente, lê o nome e imprime as informações
        std::string nomeCliente;
        ss >> nomeCliente;
        Cliente* cliente = getOrCreateCliente(nomeCliente);

        // Imprime o cabeçalho da consulta usando a função de formatação
        std::cout << formatarInt(hora, 6) << " CL " << nomeCliente << std::endl;
        cliente->printInformacao();

    } else if (tipoQuery == "PC") {
        // Se for uma consulta de pacote, lê o ID e imprime as informações
        int idPacote;
        ss >> idPacote;
        Pacote* pacote = getOrCreatePacote(idPacote);

        // Imprime o cabeçalho da consulta usando a função de formatação
        std::cout << formatarInt(hora, 6) << " PC " << formatarInt(idPacote, 3) << std::endl;
        pacote->printInformacao();
    }
}

/**
 * @brief Processa um evento e atualiza o estado do sistema.
 */
void Controlador::executarEvento(const Evento& evento) {
    Pacote* pacote = getOrCreatePacote(evento.getIdPacote());
    pacote->adicionarEvento(evento);

    if (evento.getTipo() == Evento::RG) {
        Cliente* origem = getOrCreateCliente(evento.getClienteOrigem());
        Cliente* destino = getOrCreateCliente(evento.getClienteDestino());
        origem->adicionarPacoteEnviar(pacote);
        destino->adicionarPacoteReceber(pacote);
    }
}

/**
 * @brief Adiciona um ponteiro de Pacote à árvore de pacotes.
 */
void Controlador::adicionarPacote(Pacote* pacote) {
    this->pacotes.insert(pacote);
}

/**
 * @brief Encontra um Pacote pelo ID.
 */
Pacote* Controlador::acharPacote(int id) {
    Pacote dummy(id);
    return pacotes.find(&dummy);
}

/**
 * @brief Adiciona um ponteiro de Cliente à árvore de clientes.
 */
void Controlador::adicionarCliente(Cliente* cliente) {
    this->clientes.insert(cliente);
}

/**
 * @brief Encontra um Cliente pelo nome.
 */
Cliente* Controlador::acharCliente(const std::string& nome) {
    Cliente dummy(nome);
    return clientes.find(&dummy);
}

/**
 * @brief Busca um pacote pelo ID. Se não existir, cria um novo.
 */
Pacote* Controlador::getOrCreatePacote(int id) {
    Pacote* pacote = acharPacote(id);
    if (pacote == nullptr) {
        pacote = new Pacote(id);
        adicionarPacote(pacote);
    }
    return pacote;
}

/**
 * @brief Busca um cliente pelo nome. Se não existir, cria um novo.
 */
Cliente* Controlador::getOrCreateCliente(const std::string& nome) {
    Cliente* cliente = acharCliente(nome);
    if (cliente == nullptr) {
        cliente = new Cliente(nome);
        adicionarCliente(cliente);
    }
    return cliente;
}
