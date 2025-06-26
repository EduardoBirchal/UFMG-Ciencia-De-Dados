#ifndef ARMAZEM_HPP
#define ARMAZEM_HPP

#include "secao.hpp"
#include "linkedList.hpp"
#include "stack.hpp"
#include "pacote.hpp"

// Declaração antecipada para evitar dependência circular
class Escalonador;

// Representa um armazém com seções de armazenamento
class Armazem {
private:
    int id;
    int custoRemocao; // Custo associado à remoção de um pacote
    Escalonador* escalonador; // Ponteiro para o escalonador principal
    linkedList<Secao*> secoes; // Lista de ponteiros para Secao
    stack<Pacote*> pilhaAuxiliar; // Pilha auxiliar para remoção de pacotes

    // Método auxiliar para encontrar uma seção pelo ID ou criar uma nova se não existir
    Secao* findOrCreateSecao(int id_secao);

    // Método auxiliar para encontrar uma seção apenas pelo ID
    Secao* findSecao(int id_secao) const;

public:
    // Construtor
    explicit Armazem(int id_armazem, int custo_remocao, Escalonador* esc);

    // Destrutor
    ~Armazem();
    
    // Adiciona uma nova seção ao armazém
    void addSecao(int id_secao);

    // Adiciona um pacote a uma seção
    void adicionarPacote(Pacote* pacote, int id_secao_alvo);

    // Remove um pacote de uma seção
    Pacote* removerPacote(int id_secao);

    // Remove e retorna o topo da pilha auxiliar
    Pacote* popAuxiliar();

    // Transfere um pacote de uma seção pra pilha auxiliar
    void transfereParaAuxiliar(int id_secao);

    // Agenda a remoção de todos os pacotes de uma seção. Retorna a nova hora atual
    int realocarSecao(int id_secao, int horaInicial);

    // Agenda o envio dos pacotes da pilha auxiliar
    void encaminhaPilhaAuxiliar(int horaAtual);

    // Processa todas as seções não-vazias para encaminhamento
    void encaminharPacotes(int horaAtual);

    // Verifica se todas as seções do armazém estão vazias
    bool secoesVazias() const;

    // MUDANÇA: Novo método público para processar uma única seção
    void processarEncaminhamentoSecao(int id_secao, int horaAtual);

    // MUDANÇA: Novo método público para encontrar a próxima seção com pacotes
    int findProximaSecaoNaoVazia(int idSecaoAnterior) const;

    // NOVO: Imprime os IDs de todas as seções do armazém
    void imprimirSecoes() const;

    // Getters
    int getID() const;
    int getNumSecoes() const;

    // NOVO: Retorna uma referência constante para a lista de seções.
    // Permite a leitura do estado das seções sem permitir modificações externas.
    const linkedList<Secao*>& getSecoes() const { return secoes; }
};

#endif // ARMAZEM_HPP