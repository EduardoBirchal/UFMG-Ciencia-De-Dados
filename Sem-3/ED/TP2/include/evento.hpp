#ifndef EVENTO_HPP
#define EVENTO_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include "pacote.hpp"

// Declaração antecipada para quebrar a dependência circular com Escalonador
class Escalonador; 

// Classe de evento base, agora virtual
class Evento {
public:
    unsigned long long int chave; 
    Pacote* pacote;                
    int idOrigem;
    int idDestino;
    int idSecao;
    int horaAgendada;
    int valorTipo; // Último dígito da chave
    Escalonador* escalonador;

    // Construtor
    Evento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);

    // Destrutor virtual para permitir herança segura
    virtual ~Evento() = default;

    // Define a chave de prioridade do evento
    void setChave(unsigned long long int novaChave);

    // Operador de comparação para o MinHeap ordenar os eventos
    bool operator<(const Evento& outro) const;

    // Método de simulação, a ser sobrescrito por classes filhas
    virtual void simular();

    // Faz o log do evento
    virtual void imprimir();

    // Imprime a hora, com dígitos à esquerda
    static void printHora(unsigned long long int hora);

    // Imprime o ID, com dígitos à esquerda
    static void printID(int id);
};

// --- Herdeiros de Evento ---

class EventoPostagem : public Evento {
public:
    EventoPostagem(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);
    void imprimir() override;
    void simular() override;
};

class EventoArmazenamento : public Evento {
public:
    EventoArmazenamento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);
    void imprimir() override;
    void simular() override;
};

class EventoEnviarPacote : public Evento {
public:
    EventoEnviarPacote(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);
    void imprimir() override;
    void simular() override;
};

class EventoRearmazenamento : public Evento {
public:
    EventoRearmazenamento(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);
    void imprimir() override;
    void simular() override;
};

class EventoEntrega : public Evento {
public:
    EventoEntrega(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);
    void imprimir() override;
    void simular() override;
};

// Evento para acionar a lógica de transporte de um armazém
class EventoAcionarTransporte : public Evento {
public:
    EventoAcionarTransporte(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);
    void imprimir() override;
    void simular() override;
};

// NOVO: Evento para acionar a lógica de encaminhamento de um armazém específico
class EventoAcionarArmazem : public Evento {
public:
    EventoAcionarArmazem(unsigned long long int c, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc);
    void imprimir() override;
    void simular() override;
};


// Wrapper para ponteiros de Evento, para o MinHeap comparar corretamente
struct EventoPtr {
    Evento* ptr;
    bool operator<(const EventoPtr& outro) const {
        return this->ptr->chave < outro.ptr->chave;
    }
};

#endif // EVENTO_HPP