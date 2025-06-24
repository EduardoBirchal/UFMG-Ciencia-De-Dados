#ifndef EVENTO_HPP
#define EVENTO_HPP

#include "pacote.hpp"

// Declaração antecipada para quebrar a dependência circular com Escalonador
class Escalonador; 

// Enum para os diferentes tipos de evento no sistema
enum TipoEvento {
    Postagem,
    Armazenamento,
    Remocao,
    Transporte,
    PilhaAuxiliar,
    Entrega
};

// Classe de evento base, agora virtual
class Evento {
public:
    unsigned long long int chave; 
    TipoEvento tipo;              
    Pacote* pacote;                
    int idOrigem;
    int idDestino;
    int idSecao;
    int horaAgendada;
    Escalonador* escalonador;

    // Construtor
    Evento(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);

    // Destrutor virtual para permitir herança segura
    virtual ~Evento() = default;

    // Define a chave de prioridade do evento
    void setChave(unsigned long long int novaChave);

    // Operador de comparação para o MinHeap ordenar os eventos
    bool operator<(const Evento& outro) const;

    // Método de simulação, a ser sobrescrito por classes filhas
    virtual void simular();
};

// --- Herdeiros de Evento ---

// Evento que representa a postagem inicial de um pacote na rede
class EventoPostagem : public Evento {
public:
    // Construtor que repassa os argumentos para a classe base Evento
    EventoPostagem(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);

    // Sobrescreve o método de simulação
    void simular() override;
};

// Evento que representa a chegada de um pacote em um armazém para ser armazenado
class EventoArmazenamento : public Evento {
public:
    // Construtor que repassa os argumentos para a classe base Evento
    EventoArmazenamento(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);

    // Sobrescreve o método de simulação
    void simular() override;
};


// Wrapper para ponteiros de Evento, para o MinHeap comparar corretamente
struct EventoPtr {
    Evento* ptr;

    // Compara os eventos pela chave, não pelo endereço do ponteiro
    bool operator<(const EventoPtr& outro) const {
        return this->ptr->chave < outro.ptr->chave;
    }
};

#endif // EVENTO_HPP
