#ifndef ARMAZEM_HPP
#define ARMAZEM_HPP

#include "stack.hpp"
#include "pacote.hpp"

// Representa um armazém com seções de armazenamento
class Armazem {
private:
    int id;
    linkedList<stack<Pacote*>*> secoes;

public:
    // Construtor
    Armazem(int id_armazem, int num_secoes);

    // Destrutor
    ~Armazem();

    // Adiciona um pacote a uma seção
    void adicionarPacote(Pacote* pacote);

    // Remove um pacote de uma seção
    Pacote* removerPacote(int id_secao);

    // Getters
    int getID() const;
    int getNumSecoes() const;
};

#endif // ARMAZEM_HPP
