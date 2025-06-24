#ifndef ARMAZEM_HPP
#define ARMAZEM_HPP

#include "secao.hpp"
#include "linkedList.hpp"

// Representa um armazém com seções de armazenamento
class Armazem {
private:
    int id;
    linkedList<Secao*> secoes; // Lista de ponteiros para Secao

    // Método auxiliar para encontrar uma seção pelo ID ou criar uma nova se não existir
    Secao* findOrCreateSecao(int id_secao);

public:
    // Construtor
    explicit Armazem(int id_armazem);

    // Destrutor
    ~Armazem();
    
    // Adiciona uma nova seção ao armazém
    void addSecao(int id_secao); // Alterado para receber um ID específico

    // Adiciona um pacote a uma seção
    void adicionarPacote(Pacote* pacote);

    // Remove um pacote de uma seção
    Pacote* removerPacote(int id_secao);

    // Getters
    int getID() const;
    int getNumSecoes() const;
};

#endif // ARMAZEM_HPP
