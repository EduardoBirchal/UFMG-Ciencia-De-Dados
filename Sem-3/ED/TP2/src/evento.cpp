#include "evento.hpp"
#include "escalonador.hpp" // Necessário para acessar os métodos e a rede do escalonador
#include "armazem.hpp"      // Necessário para a função adicionarPacote()

// --- Implementação dos Métodos da Classe Base 'Evento' ---

Evento::Evento(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc) : 
    chave(c), tipo(t), pacote(p), idOrigem(origem), idDestino(destino), idSecao(secao), horaAgendada(hora), escalonador(esc) {}

void Evento::setChave(unsigned long long int novaChave) {
    this->chave = novaChave;
}

bool Evento::operator<(const Evento& outro) const {
    return this->chave < outro.chave;
}

void Evento::simular() {
    // O comportamento padrão não faz nada
}

// --- Implementação dos Métodos da Classe 'EventoPostagem' ---

EventoPostagem::EventoPostagem(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, t, p, origem, destino, secao, hora, esc) {}

void EventoPostagem::simular() {
    // Garante que os ponteiros necessários são válidos
    if (!escalonador || !pacote) return;

    Rede* redeDoSistema = escalonador->getRede();
    if (!redeDoSistema) return;

    // A lógica de uma postagem é calcular a rota e adicionar o pacote ao armazém inicial
    redeDoSistema->calcularRota(this->pacote);

    Armazem* armazemOrigem = redeDoSistema->getArmazem(this->idOrigem);
    if (armazemOrigem) {
        armazemOrigem->adicionarPacote(this->pacote);
    }
}

// --- Implementação dos Métodos da Classe 'EventoArmazenamento' ---

EventoArmazenamento::EventoArmazenamento(unsigned long long int c, TipoEvento t, Pacote* p, int origem, int destino, int secao, int hora, Escalonador* esc)
    : Evento(c, t, p, origem, destino, secao, hora, esc) {}

void EventoArmazenamento::simular() {
    // Garante que os ponteiros necessários são válidos
    if (!escalonador || !pacote) return;

    Rede* redeDoSistema = escalonador->getRede();
    if (!redeDoSistema) return;

    // A lógica de um evento de armazenamento é adicionar o pacote ao armazém de destino do evento
    Armazem* armazemDestino = redeDoSistema->getArmazem(this->idDestino);
    if (armazemDestino) {
        armazemDestino->adicionarPacote(this->pacote);
    }
}
