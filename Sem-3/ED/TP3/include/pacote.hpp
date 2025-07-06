#ifndef PACOTE_HPP
#define PACOTE_HPP

#include "linkedList.hpp" // Para usar a lista encadeada
#include "Evento.hpp"       // Para usar a classe Evento

/**
 * @class Pacote
 * @brief Representa um pacote com um identificador único e seu histórico de eventos
 */
class Pacote {
private:
    int id; // O identificador único do pacote
    linkedList<Evento> historicoEventos; // Lista de eventos associados ao pacote

public:
    /**
     * @brief Construtor padrão e parametrizado
     * @param id O ID inicial do pacote (padrão é 0)
     */
    Pacote(int id = 0);

    /**
     * @brief Destrutor
     */
    ~Pacote();

    bool operator<(const Pacote& outro) const;
    bool operator==(const Pacote& outro) const;

    /**
     * @brief Imprime o número de eventos e depois cada evento do histórico do pacote.
     */
    void printInformacao() const;

    // --- Getters ---
    int getId() const;
    Evento& getPrimeiroEvento();
    Evento& getUltimoEvento();
    linkedList<Evento>& getHistoricoEventos();

    // --- Setter ---
    void setId(int id);

    // --- Outros Métodos ---
    void adicionarEvento(const Evento& evento);
};

#endif // PACOTE_HPP
