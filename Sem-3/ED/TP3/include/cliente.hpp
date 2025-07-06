#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include "linkedList.hpp"
#include "pacote.hpp"

class Pacote; // Declaração avançada para o ponteiro

/**
 * @class Cliente
 * @brief Representa um cliente no sistema de logística
 */
class Cliente {
private:
    std::string nome;
    linkedList<Pacote*> pacotesEnviar;
    linkedList<Pacote*> pacotesReceber;

    /**
     * @brief Função auxiliar para Radix Sort: encontra o maior valor de chave.
     */
    static long long getMax(parOrdenado<Pacote*, long long> arr[], int n);

    /**
     * @brief Função auxiliar para Radix Sort: usa Counting Sort para ordenar por dígito.
     */
    static void countingSort(parOrdenado<Pacote*, long long> arr[], int n, long long exp);

public:
    /**
     * @brief Construtor que inicializa o cliente com um nome
     * @param nome O nome do cliente
     */
    Cliente(const std::string& nome = "");

    /**
     * @brief Destrutor
     */
    ~Cliente();

    bool operator<(const Cliente& outro) const;
    bool operator==(const Cliente& outro) const;

    /**
     * @brief Imprime informações sobre os pacotes do cliente.
     * Imprime o primeiro e o último evento de cada pacote enviado e recebido.
     */
    void printInformacao() const;

    /**
     * @brief Calcula uma chave única para um par pacote-evento.
     * @param pacote Ponteiro para o pacote.
     * @param evento O evento.
     * @return Um número de chave (long long).
     */
    static long long calcularChave(Pacote* pacote, const Evento& evento);

    /**
    * @brief Ordena um array de pares (pacote, chave) usando Radix Sort com base na chave.
    * @param arr O array a ser ordenado.
    * @param n O tamanho do array.
    */
    static void radixSort(parOrdenado<Pacote*, long long> arr[], int n);

    // --- Getters ---
    std::string getNome() const;
    linkedList<Pacote*>& getPacotesEnviar();
    linkedList<Pacote*>& getPacotesReceber();

    // --- Setters ---
    void setNome(const std::string& nome);

    // --- Métodos de Manipulação ---
    void adicionarPacoteEnviar(Pacote* pacote);
    void adicionarPacoteReceber(Pacote* pacote);
};

#endif // CLIENTE_HPP
