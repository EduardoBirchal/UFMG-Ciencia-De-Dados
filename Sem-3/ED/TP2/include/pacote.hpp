#ifndef PACOTE_HPP
#define PACOTE_HPP

#include <string>

// Representa os estados de um pacote
enum EstadoPacote {
    inicial,
    emTransporte,
    sendoArmazenado,
    armazenado,
    sendoRetirado,
    entregue
};

class Pacote {
private:
    int id;
    int horaPostagem;
    int idOrigem;
    int idDestino;
    int idAtual;
    int idSecaoAtual;
    EstadoPacote estado;

public:
    // Construtor
    Pacote(int id_pacote, int hora_postagem, int id_origem, int id_destino);

    // Destrutor
    ~Pacote();

    // Getters
    int getID() const;
    int getHoraPostagem() const;
    int getOrigem() const;
    int getDestino() const;
    int getAtual() const;
    int getSecaoAtual() const;
    EstadoPacote getEstado() const;
    
    // Setters
    void setEstado(EstadoPacote novo_estado);
    void setAtual(int id_atual);
    void setSecaoAtual(int id_secao_atual);
};

#endif