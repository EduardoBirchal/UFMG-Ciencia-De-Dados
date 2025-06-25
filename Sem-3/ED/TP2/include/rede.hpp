#ifndef REDE_HPP
#define REDE_HPP

#include "armazem.hpp"
#include "pacote.hpp"

class Escalonador; // Declaração antecipada

// Gerencia a rede de armazéns e as rotas
class Rede {
private:
    int maxArmazens;
    int numArmazensAtual;
    int maxTransporte;
    int custoRemocao; // Custo padrão para remoção em todos os armazéns
    Escalonador* escalonador; // Ponteiro para o escalonador
    Armazem** armazens;
    int** matrizAdjacencia;

    // Método auxiliar para encontrar o índice de um armazém pelo seu ID
    int findIndex(int id_armazem) const;

public:
    // Construtor: define a capacidade e os custos da rede
    Rede(int max_armazens, int max_transporte, int custo_remocao);

    // Destrutor
    ~Rede();

    // Define o ponteiro para o escalonador (para quebrar dependência circular de construção)
    void setEscalonador(Escalonador* esc);

    // Cria e adiciona um armazém à rede
    void addArmazem(int id_armazem);

    // Adiciona uma nova seção a um armazém específico
    void addSecao(int id_armazem);

    // Adiciona uma conexão entre dois armazéns (usando seus IDs)
    void addConexao(int id_origem, int id_destino);

    // Calcula a rota mais curta para um pacote e a armazena nele
    void calcularRota(Pacote* pacote);

    // Retorna um armazém pelo seu ID
    Armazem* getArmazem(int id);

    // Retorna um armazém pelo seu índice no array interno
    Armazem* getArmazemPeloIndice(int index) const;

    // Retorna a capacidade máxima de transporte
    int getMaxTransporte() const;

    // Retorna o número de armazéns atualmente na rede
    int getNumArmazens() const;
};

#endif // REDE_HPP
