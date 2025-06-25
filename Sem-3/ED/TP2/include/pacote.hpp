#ifndef PACOTE_HPP
#define PACOTE_HPP

#include <string>
#include "queue.hpp" // Inclusão da classe Queue

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
    int idSequencial;   // ID gerado pelo programa
    int idEntrada;      // ID recebido na entrada
    int horaPostagem;
    int idOrigem;
    int idDestinoFinal;
    int idDestinoAtual;
    int idArmazemAtual;
    int idSecaoAtual;
    EstadoPacote estado;
    Queue<int> rotaArmazens; // Fila para armazenar a rota

    // Método privado para lidar com a entrega bem-sucedida do pacote
    void pacoteEntregue();

public:
    // Construtor atualizado
    Pacote(int hora_postagem, int id_entrada, int id_origem, int id_destino_final, int id_sequencial);

    // Destrutor
    ~Pacote();

    // --- Getters ---
    int getID() const;
    int getIDEntrada() const;
    int getHoraPostagem() const;
    int getOrigem() const;
    int getDestinoFinal() const;
    int getDestinoAtual() const;
    int getArmazemAtual() const;
    int getSecaoAtual() const;
    EstadoPacote getEstado() const;
    Queue<int>& getRota();
    
    // --- Setters ---
    void setEstado(EstadoPacote novo_estado);
    void setArmazemAtual(int id_atual);
    void setSecaoAtual(int id_secao_atual);
    void setDestinoAtual(int id_destino);
    
    // --- Métodos de Lógica ---
    // Atualiza o destino atual com o próximo item da fila de rota
    void atualizarDestinoAtual();
};

#endif
