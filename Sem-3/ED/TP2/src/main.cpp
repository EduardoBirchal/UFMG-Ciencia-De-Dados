#include <iostream>
#include <vector>
#include <utility>
#include <random>    // Para geração de números aleatórios de alta qualidade
#include <ctime>
#include <string>
#include <stdexcept>

#include "escalonador.hpp"
#include "rede.hpp"
#include "pacote.hpp"
#include "evento.hpp"
#include "armazem.hpp"
#include "estatisticas.hpp"

// --- FUNÇÃO AUXILIAR PARA TOPOLOGIA DE REDE ---
void configurarRedePadrao(Rede& rede, int numArmazens) {
    if (numArmazens < 10) {
        throw std::invalid_argument("A topologia complexa requer pelo menos 10 armazens.");
    }
    for (int i = 0; i < numArmazens; ++i) {
        rede.addArmazem(i);
    }
    rede.addConexao(0, 1);
    rede.addConexao(0, 2);
    rede.addConexao(2, 3);
    rede.addConexao(3, 4);
    rede.addConexao(4, 2);
    rede.addConexao(1, 5);
    rede.addConexao(5, 6);
    rede.addConexao(6, 7);
    rede.addConexao(7, 5);
    rede.addConexao(0, 8);
    rede.addConexao(1, 9);
    rede.addConexao(8, 9);
}

// --- NOVA FUNÇÃO AUXILIAR PARA GERAÇÃO DE PACOTES ---
// Gera e agenda 'num_pacotes' com origens e destinos aleatórios.
void gerarPacotesAleatorios(Escalonador& escalonador, int num_pacotes, int num_armazens) {
    // Usa std::random_device para obter uma semente verdadeiramente aleatória do sistema.
    std::random_device rd;
    // Alimenta o gerador Mersenne Twister com a semente aleatória.
    std::mt19937 gen(rd());
    // Cria uma distribuição uniforme entre 0 e o número de armazéns - 1.
    std::uniform_int_distribution<> distrib(0, num_armazens - 1);

    for (int i = 0; i < num_pacotes; ++i) {
        int origem = distrib(gen);
        int destino = distrib(gen);
        // Garante que o pacote não tem origem e destino iguais.
        while (origem == destino) {
            destino = distrib(gen);
        }
        
        Pacote* p = new Pacote(i * 5, i + 1000, origem, destino, i);
        escalonador.agendar<EventoPostagem>(p, p->getOrigem(), p->getDestinoFinal(), 0, p->getHoraPostagem());
    }
}


// --- Análise 1: Variação do Número de Pacotes ---
void analisePacotes() {
    const int NUM_ARMAZENS = 10;
    const int PACOTE_INICIAL = 10;
    const int PACOTE_MAXIMO = 300;
    const int INCREMENTO_PACOTES = 6;
    const std::string NOME_ARQUIVO_SAIDA = "analise_pacotes.csv";
    
    std::vector<std::pair<int, int>> resultados;
    std::cout << "Iniciando analise por NUMERO DE PACOTES com " << NUM_ARMAZENS << " armazens..." << std::endl;

    for (int num_pacotes = PACOTE_INICIAL; num_pacotes <= PACOTE_MAXIMO; num_pacotes += INCREMENTO_PACOTES) {
        std::cout << "Rodando simulacao com " << num_pacotes << " pacotes..." << std::endl;
        
        Estatisticas::instancia().reset();
        Rede rede(NUM_ARMAZENS, 2, 1);
        Escalonador escalonador(num_pacotes * 100, num_pacotes, 20, 110, &rede);
        configurarRedePadrao(rede, NUM_ARMAZENS);
        
        // MODIFICADO: Chama a nova função para gerar pacotes aleatórios.
        gerarPacotesAleatorios(escalonador, num_pacotes, rede.getNumArmazens());

        while (!escalonador.vazio()) {
            escalonador.simularProximo();
        }
        
        resultados.push_back({num_pacotes, Estatisticas::instancia().getTotalOperacoes()});
    }

    Estatisticas::instancia().salvarAnaliseEmCSV(NOME_ARQUIVO_SAIDA, "Numero de Pacotes", resultados);
    std::cout << "Analise de pacotes concluida." << std::endl;
}

// --- Análise 2: Variação da Capacidade de Transporte ---
void analiseCapacidade() {
    const int NUM_ARMAZENS = 10;
    const int CAPACIDADE_INICIAL = 1;
    const int CAPACIDADE_MAXIMA = 50;
    const int INCREMENTO_CAPACIDADE = 1;
    const int NUM_PACOTES_FIXO = 100;
    const std::string NOME_ARQUIVO_SAIDA = "analise_capacidade.csv";

    std::vector<std::pair<int, int>> resultados;
    std::cout << "Iniciando analise por CAPACIDADE DE TRANSPORTE com " << NUM_ARMAZENS << " armazens..." << std::endl;

    for (int capacidade = CAPACIDADE_INICIAL; capacidade <= CAPACIDADE_MAXIMA; capacidade += INCREMENTO_CAPACIDADE) {
        std::cout << "Rodando simulacao com capacidade de transporte " << capacidade << "..." << std::endl;
        
        Estatisticas::instancia().reset();
        Rede rede(NUM_ARMAZENS, capacidade, 1);
        Escalonador escalonador(NUM_PACOTES_FIXO * 100, NUM_PACOTES_FIXO, 20, 110, &rede);
        configurarRedePadrao(rede, NUM_ARMAZENS);
        
        // MODIFICADO: Chama a nova função para gerar pacotes aleatórios.
        gerarPacotesAleatorios(escalonador, NUM_PACOTES_FIXO, rede.getNumArmazens());

        while (!escalonador.vazio()) {
            escalonador.simularProximo();
        }
        
        resultados.push_back({capacidade, Estatisticas::instancia().getTotalOperacoes()});
    }

    Estatisticas::instancia().salvarAnaliseEmCSV(NOME_ARQUIVO_SAIDA, "Capacidade de Transporte", resultados);
    std::cout << "Analise de capacidade concluida." << std::endl;
}


int main(int argc, char* argv[]) {
    // Seleciona o tipo de análise com base nos argumentos de linha de comando
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <tipo_analise>" << std::endl;
        std::cerr << "Tipos disponiveis: 'pacotes' ou 'capacidade'" << std::endl;
        return 1;
    }

    std::string tipoAnalise = argv[1];

    if (tipoAnalise == "pacotes") {
        analisePacotes();
    } else if (tipoAnalise == "capacidade") {
        analiseCapacidade();
    } else {
        std::cerr << "Erro: tipo de analise '" << tipoAnalise << "' desconhecido." << std::endl;
        std::cerr << "Tipos disponiveis: 'pacotes' ou 'capacidade'" << std::endl;
        return 1;
    }

    return 0;
}
