#ifndef ESCALONADOR_HPP
#define ESCALONADOR_HPP

#include "minheap.hpp"
#include "pacote.hpp" 
#include <type_traits> // Para std::is_base_of

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

    // Construtor com parâmetros
    Evento(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora) : 
        chave(c), tipo(t), pacote(p), idOrigem(origem), idDestino(destino), idSecao(secao), horaAgendada(hora) {}

    // Destrutor virtual para permitir herança segura
    virtual ~Evento() = default;

    // Define a chave de prioridade do evento
    void setChave(unsigned long long int novaChave) {
        this->chave = novaChave;
    }

    // Operador de comparação para o MinHeap ordenar os eventos
    bool operator<(const Evento& outro) const {
        return this->chave < outro.chave;
    }

    // Método de simulação, pode ser sobrescrito por classes filhas
    virtual void simular() {
        // Comportamento padrão não faz nada
    }
};

// Wrapper para ponteiros de Evento, para o MinHeap comparar corretamente
struct EventoPtr {
    Evento* ptr;

    // Compara os eventos pela chave, não pelo endereço do ponteiro
    bool operator<(const EventoPtr& outro) const {
        return this->ptr->chave < outro.ptr->chave;
    }
};


// Gerencia uma fila de eventos baseada em MinHeap
class Escalonador {
private:
    MinHeap<EventoPtr> filaDeEventos; // Fila de prioridade de ponteiros para Eventos
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

    // Agenda um novo evento de qualquer tipo que herde de Evento
    template<typename T>
    void agendar(TipoEvento tipo, Pacote* pacote, int idOrigem, int idDestino, int idSecao, int horaAgendada);

    // Retorna o próximo evento (sem remover)
    Evento* proximo() const;

    // Processa e retorna o próximo evento da fila
    Evento* processarProximo();

    // Verifica se a fila de eventos está vazia
    bool vazio() const;

    // Retorna o número de eventos agendados
    int tamanho() const;
};


// --- Implementação do Template Agendar ---
template<typename T>
void Escalonador::agendar(TipoEvento tipo, Pacote* pacote, int idOrigem, int idDestino, int idSecao, int horaAgendada) {
    // Garante que T herda de Evento
    static_assert(std::is_base_of<Evento, T>::value, "T deve ser uma classe derivada de Evento");

    // Cria evento do tipo T com uma chave temporária (0)
    Evento* novoEvento = new T(0, tipo, pacote, idOrigem, idDestino, idSecao, horaAgendada);
    
    // Calcula e define a chave correta no evento recém-criado
    novoEvento->setChave(this->calcularChave(*novoEvento));
    
    // Insere o evento na fila de prioridade
    filaDeEventos.insert({novoEvento});
}


#endif // ESCALONADOR_HPP
