#ifndef ESCALONADOR_HPP
#define ESCALONADOR_HPP

#include "minheap.hpp"
#include "evento.hpp" // Inclui a nova hierarquia de eventos
#include "rede.hpp"
#include <type_traits> // Para std::is_base_of

// MUDANÇA: Hierarquia de prioridades expandida para 4 níveis
enum class Prioridade {
    ALTA = 0,     // Para eventos de envio/rearmazenamento de pacotes
    SECAO = 1,    // Para o evento que aciona o processamento de uma seção
    ARMAZEM = 2,  // Para o evento que inicia o processamento de um armazém
    NORMAL = 3    // Para todos os outros eventos (chegadas, postagens, etc.)
};

// Gerencia uma fila de eventos baseada em MinHeap
class Escalonador {
private:
    MinHeap<EventoPtr> filaDeEventos; // Fila de prioridade de ponteiros para Eventos
    Rede* rede; // Ponteiro para a rede de armazéns
    int numPacotes;
    int latenciaTransporte;
    int intervaloTransportes;
    int horaAtual;
    bool primeiroTransporteAgendado;

    // MUDANÇA: O cálculo da chave agora considera a prioridade
    unsigned long long int calcularChave(const Evento& evento, Prioridade prioridade) const;

public:
    // Construtor
    explicit Escalonador(int maxEventos, int numPacotes, int latencia, int intervalo, Rede* rede);
    
    // Destrutor
    ~Escalonador();

    // MUDANÇA: O método agendar agora aceita um parâmetro de prioridade
    template<typename T>
    void agendar(Pacote* pacote, int idOrigem, int idDestino, int idSecao, int horaAgendada, Prioridade prioridade = Prioridade::NORMAL);

    // Retorna o próximo evento (sem remover)
    Evento* proximo() const;

    // Processa e retorna o próximo evento da fila
    Evento* processarProximo();

    // Processa, simula e deleta o próximo evento
    void simularProximo();

    // Verifica se a fila de eventos está vazia
    bool vazio() const;

    // Retorna o número de eventos agendados
    int tamanho() const;

    // Retorna um ponteiro para a rede
    Rede* getRede() const;

    // Retorna a latência do transporte
    int getLatenciaTransporte() const;

    // Retorna o intervalo entre transportes
    int getIntervaloTransportes() const;

    bool foiPrimeiroTransporteAgendado() const;
    void marcarPrimeiroTransporteAgendado();
};


// --- Implementação do Template Agendar ---
template<typename T>
void Escalonador::agendar(Pacote* pacote, int idOrigem, int idDestino, int idSecao, int horaAgendada, Prioridade prioridade) {
    static_assert(std::is_base_of<Evento, T>::value, "T deve ser uma classe derivada de Evento");

    Evento* novoEvento = new T(0, pacote, idOrigem, idDestino, idSecao, horaAgendada, this);
    
    // MUDANÇA: A prioridade é passada para o cálculo da chave
    unsigned long long int novaChave = this->calcularChave(*novoEvento, prioridade);
    novoEvento->setChave(novaChave);

    filaDeEventos.insert({novoEvento});
}


#endif // ESCALONADOR_HPP