#include "Evento.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>   // Para usar std::stringstream
#include <iomanip>   // Para usar std::setw e std::setfill
#include <stdexcept> // Para usar std::invalid_argument

// Implementação do construtor padrão
Evento::Evento() : hora(0), tipo(RG), idPacote(0), armazemOrigem(0), armazemDestino(0), secaoDestino(0) {
    // Inicializa o tipo com o valor padrão RG (Registro)
}

// Implementação do construtor de cópia
Evento::Evento(const Evento& outro)
    : hora(outro.hora),
      tipo(outro.tipo),
      idPacote(outro.idPacote),
      clienteOrigem(outro.clienteOrigem),
      clienteDestino(outro.clienteDestino),
      armazemOrigem(outro.armazemOrigem),
      armazemDestino(outro.armazemDestino),
      secaoDestino(outro.secaoDestino) {
}


// Implementação do método estático para converter string para TipoEvento
Evento::TipoEvento Evento::stringPraTipo(const std::string& str) {
    if (str == "RG") return RG;
    if (str == "AR") return AR;
    if (str == "RM") return RM;
    if (str == "UR") return UR;
    if (str == "TR") return TR;
    if (str == "EN") return EN;

    throw std::invalid_argument("String de tipo de evento desconhecida: " + str);
}

/**
 * @brief Cria um objeto Evento a partir de uma string formatada
 * @param linha A string contendo os dados do evento.
 * @return Um objeto Evento preenchido com os dados da string
 */
Evento Evento::stringPraEvento(const std::string& linha) {
    std::stringstream ss(linha);
    Evento evento;
    int hora, idPacote;
    std::string palavraEv, tipoStr;

    ss >> hora >> palavraEv >> tipoStr >> idPacote;

    if (ss.fail() || palavraEv != "EV") {
        throw std::invalid_argument("Formato de string de evento invalido. Esperado: '<hora> EV <tipo> <idPacote>...'");
    }

    evento.setHora(hora);
    evento.setTipo(stringPraTipo(tipoStr));
    evento.setIdPacote(idPacote);

    switch (evento.getTipo()) {
        case RG: {
            int armazemOrigem, armazemDestino;
            std::string clienteOrigem, clienteDestino;
            ss >> clienteOrigem >> clienteDestino >> armazemOrigem >> armazemDestino;
            if (ss.fail()) throw std::runtime_error("Erro ao ler os parametros para o evento RG");
            evento.setClienteOrigem(clienteOrigem);
            evento.setClienteDestino(clienteDestino);
            evento.setArmazemOrigem(armazemOrigem);
            evento.setArmazemDestino(armazemDestino);
            break;
        }
        case AR: case RM: case UR: {
            int armazemDestino, secaoDestino;
            ss >> armazemDestino >> secaoDestino;
            if (ss.fail()) throw std::runtime_error("Erro ao ler os parametros para o evento " + tipoStr);
            evento.setArmazemDestino(armazemDestino);
            evento.setSecaoDestino(secaoDestino);
            break;
        }
        case TR: {
            int armazemOrigem, armazemDestino;
            ss >> armazemOrigem >> armazemDestino;
            if (ss.fail()) throw std::runtime_error("Erro ao ler os parametros para o evento TR");
            evento.setArmazemOrigem(armazemOrigem);
            evento.setArmazemDestino(armazemDestino);
            break;
        }
        case EN: {
            int armazemDestino;
            ss >> armazemDestino;
            if (ss.fail()) throw std::runtime_error("Erro ao ler os parametros para o evento EN");
            evento.setArmazemDestino(armazemDestino);
            break;
        }
    }
    return evento;
}

// --- Funções de Conversão para String ---

/**
 * @brief Converte um enum TipoEvento para sua representação em string.
 */
std::string Evento::tipoPraString(TipoEvento tipo) {
    switch (tipo) {
        case RG: return "RG";
        case AR: return "AR";
        case RM: return "RM";
        case UR: return "UR";
        case TR: return "TR";
        case EN: return "EN";
        default: return ""; // Não deve acontecer
    }
}

/**
 * @brief Converte os dados do evento para uma string formatada.
 */
std::string Evento::eventoPraString() const {
    std::stringstream ss;
    ss << formatarInt(this->hora, 7) 
       << " EV " << tipoPraString(this->tipo) << " " 
       << formatarInt(this->idPacote, 3);

    switch (this->tipo) {
        case RG:
            ss << " " << this->clienteOrigem << " " << this->clienteDestino
               << " " << formatarInt(this->armazemOrigem, 3) 
               << " " << formatarInt(this->armazemDestino, 3);
            break;
        case AR:
        case RM:
        case UR:
            ss << " " << formatarInt(this->armazemDestino, 3) 
               << " " << formatarInt(this->secaoDestino, 3);
            break;
        case TR:
            ss << " " << formatarInt(this->armazemOrigem, 3) 
               << " " << formatarInt(this->armazemDestino, 3);
            break;
        case EN:
            ss << " " << formatarInt(this->armazemDestino, 3);
            break;
    }
    return ss.str();
}


// --- Implementação dos Getters ---

int Evento::getHora() const { return this->hora; }
Evento::TipoEvento Evento::getTipo() const { return this->tipo; }
int Evento::getIdPacote() const { return this->idPacote; }
std::string Evento::getClienteOrigem() const { return this->clienteOrigem; }
std::string Evento::getClienteDestino() const { return this->clienteDestino; }
int Evento::getArmazemOrigem() const { return this->armazemOrigem; }
int Evento::getArmazemDestino() const { return this->armazemDestino; }
int Evento::getSecaoDestino() const { return this->secaoDestino; }

// --- Implementação dos Setters ---

void Evento::setHora(int hora) { this->hora = hora; }
void Evento::setTipo(Evento::TipoEvento tipo) { this->tipo = tipo; }
void Evento::setIdPacote(int idPacote) { this->idPacote = idPacote; }
void Evento::setClienteOrigem(const std::string& clienteOrigem) { this->clienteOrigem = clienteOrigem; }
void Evento::setClienteDestino(const std::string& clienteDestino) { this->clienteDestino = clienteDestino; }
void Evento::setArmazemOrigem(int armazemOrigem) { this->armazemOrigem = armazemOrigem; }
void Evento::setArmazemDestino(int armazemDestino) { this->armazemDestino = armazemDestino; }
void Evento::setSecaoDestino(int secaoDestino) { this->secaoDestino = secaoDestino; }
