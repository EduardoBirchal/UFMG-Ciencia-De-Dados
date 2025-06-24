#ifndef ESCALONADOR_HPP
#define ESCALONADOR_HPP

#include "minheap.hpp"
#include "pacote.hpp" // Dependência para a carga útil do evento

// Enum para os diferentes tipos de evento no sistema
enum TipoEvento {
    Postagem,
    Armazenamento,
    Remocao,
    Transporte,
    PilhaAuxiliar,
    Entrega
};

// Classe de evento, agora não é mais um template
class Evento {
public:
    unsigned long long int chave; 
    TipoEvento tipo;              
    Pacote* pacote;                
    int idOrigem;
    int idDestino;
    int idSecao;
    int horaAgendada;

    // Construtor padrão
    Evento() : chave(0), tipo(Postagem), pacote(nullptr), idOrigem(-1), idDestino(-1), idSecao(-1), horaAgendada(-1) {}

    // Construtor com parâmetros
    Evento(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora) : 
        chave(c), tipo(t), pacote(p), idOrigem(origem), idDestino(destino), idSecao(secao), horaAgendada(hora) {}

    // Operador de comparação para o MinHeap ordenar os eventos
    bool operator<(const Evento& outro) const {
        return this->chave < outro.chave;
    }
};


// Gerencia uma fila de eventos baseada em MinHeap
class Escalonador {
private:
    MinHeap<Evento> filaDeEventos; // Fila de prioridade de Eventos
    int numPacotes;
    int latenciaTransporte;
    int intervaloTransportes;

    // Calcula a chave de prioridade para um evento
    unsigned long long int calcularChave(const Evento& evento) const;

public:
    // Construtor
    explicit Escalonador(int maxEventos, int numPacotes, int latencia, int intervalo);
    
    // Destrutor
    ~Escalonador();

    // Agenda um novo evento para execução futura
    void agendar(TipoEvento tipo, Pacote* pacote, int idOrigem, int idDestino, int idSecao, int horaAgendada);

    // Retorna o próximo evento (sem remover)
    Evento proximo() const;

    // Processa e retorna o próximo evento da fila
    Evento processarProximo();

    // Verifica se a fila de eventos está vazia
    bool vazio() const;

    // Retorna o número de eventos agendados
    int tamanho() const;
};

#endif // ESCALONADOR_HPP
