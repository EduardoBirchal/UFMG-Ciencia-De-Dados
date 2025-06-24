#ifndef REDE_HPP
#define REDE_HPP

#include "armazem.hpp"
#include "pacote.hpp" // Incluído para usar a classe Pacote

// Gerencia a rede de armazéns e as rotas
class Rede {
private:
    int maxArmazens;        // Capacidade máxima de armazéns na rede
    int numArmazensAtual;   // Número atual de armazéns na rede
    int maxTransporte;
    Armazem** armazens;
    int** matrizAdjacencia;

    // Método auxiliar para encontrar o índice de um armazém pelo seu ID
    int findIndex(int id_armazem) const;

public:
    // Construtor: define a capacidade máxima da rede
    Rede(int max_armazens, int max_transporte);

    // Destrutor
    ~Rede();

    // Adiciona um armazém à rede
    void addArmazem(Armazem* armazem);

    // Adiciona uma nova seção a um armazém específico
    void addSecao(int id_armazem);

    // Adiciona uma conexão entre dois armazéns (usando seus IDs)
    void addConexao(int id_origem, int id_destino);

    // Calcula a rota mais curta para um pacote e a armazena nele
    void calcularRota(Pacote* pacote);

    // Retorna um armazém pelo seu ID
    Armazem* getArmazem(int id);
};

#endif // REDE_HPP
