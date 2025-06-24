#ifndef ESCALONADOR_HPP
#define ESCALONADOR_HPP

#include "minheap.hpp"
#include "evento.hpp" // Inclui a nova hierarquia de eventos
#include "rede.hpp"
#include <type_traits> // Para std::is_base_of

// Gerencia uma fila de eventos baseada em MinHeap
class Escalonador {
private:
    MinHeap<EventoPtr> filaDeEventos; // Fila de prioridade de ponteiros para Eventos
    Rede* rede; // Ponteiro para a rede de armazéns
    int numPacotes;
    int latenciaTransporte;
    int intervaloTransportes;

    // Calcula a chave de prioridade para um evento
    unsigned long long int calcularChave(const Evento& evento) const;

public:
    // Construtor
    explicit Escalonador(int maxEventos, int numPacotes, int latencia, int intervalo, Rede* rede);
    
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

    // Retorna um ponteiro para a rede
    Rede* getRede() const;
};


// --- Implementação do Template Agendar ---
template<typename T>
void Escalonador::agendar(TipoEvento tipo, Pacote* pacote, int idOrigem, int idDestino, int idSecao, int horaAgendada) {
    // Garante que T herda de Evento
    static_assert(std::is_base_of<Evento, T>::value, "T deve ser uma classe derivada de Evento");

    // Cria evento do tipo T com chave temporária (0) e passa o ponteiro do escalonador ('this')
    Evento* novoEvento = new T(0, tipo, pacote, idOrigem, idDestino, idSecao, horaAgendada, this);
    
    // Calcula e define a chave correta no evento recém-criado
    novoEvento->setChave(this->calcularChave(*novoEvento));
    
    // Insere o evento na fila de prioridade
    filaDeEventos.insert({novoEvento});
}


#endif // ESCALONADOR_HPP
