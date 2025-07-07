#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>    // Para medição de tempo
#include <sstream>   // Para redirecionamento de output

#include "Controlador.hpp"
#include "Evento.hpp"

// Constante padrão para os modos de análise individuais
const int DEFAULT_ANALYSIS_N = 15;

// Protótipos das funções atualizados
void gerarQueries(std::vector<std::string>& queries, int numPacotesPorCliente, int numClientes, int numEventosPorPacote);
void analiseClientes(int n_max_clientes, int default_n);
void analisePacotes(int n_max_pacotes, int default_n);
void analiseEventos(int n_max_eventos, int default_n);

/**
 * @brief Função principal com múltiplos modos de operação.
 *
 * Modos disponíveis:
 * 1. Execução:   ./meu_programa <arquivo_de_entrada.txt>
 * 2. Geração:    ./meu_programa -g <pacotes_por_cliente> <clientes> <eventos_por_pacote> <arquivo_saida.txt>
 * 3. Análise CL: ./meu_programa -ac <max_clientes>
 * 4. Análise PC: ./meu_programa -ap <max_pacotes_por_cliente>
 * 5. Análise EV: ./meu_programa -ae <max_eventos_por_pacote>
 * 6. Análise Completa: ./meu_programa -a <n_max> <default_val>
 */
int main(int argc, char* argv[]) {
    // --- Modo de Análise Completa ---
    if (argc == 4 && std::string(argv[1]) == "-a") {
        try {
            int n_max = std::stoi(argv[2]);
            int default_val = std::stoi(argv[3]);
            if (n_max < 2 || default_val < 1) {
                std::cerr << "Erro: Para analise completa, 'n_max' deve ser >= 2 e 'default_val' >= 1." << std::endl;
                return 1;
            }
            std::cout << "--- INICIANDO ANALISE COMPLETA COM N_MAX = " << n_max << " E DEFAULT = " << default_val << " ---" << std::endl << std::endl;
            analiseEventos(n_max, default_val);
            std::cout << std::endl;
            analisePacotes(n_max, default_val);
            std::cout << std::endl;
            analiseClientes(n_max, default_val);
            std::cout << std::endl << "--- ANALISE COMPLETA FINALIZADA ---" << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro: Argumentos numericos invalidos para analise completa." << std::endl;
            return 1;
        }
        return 0;
    }

    // --- Modo de Análise de Eventos ---
    if (argc == 3 && std::string(argv[1]) == "-ae") {
        try {
            int n_max_eventos = std::stoi(argv[2]);
            if (n_max_eventos < 1) {
                std::cerr << "Erro: O numero maximo de eventos para analise deve ser pelo menos 1." << std::endl;
                return 1;
            }
            analiseEventos(n_max_eventos, DEFAULT_ANALYSIS_N);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro: Argumento numerico invalido para analise de eventos." << std::endl;
            return 1;
        }
        return 0;
    }

    // --- Modo de Análise de Pacotes ---
    if (argc == 3 && std::string(argv[1]) == "-ap") {
        try {
            int n_max_pacotes = std::stoi(argv[2]);
            if (n_max_pacotes < 1) {
                std::cerr << "Erro: O numero maximo de pacotes para analise deve ser pelo menos 1." << std::endl;
                return 1;
            }
            analisePacotes(n_max_pacotes, DEFAULT_ANALYSIS_N);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro: Argumento numerico invalido para analise de pacotes." << std::endl;
            return 1;
        }
        return 0;
    }

    // --- Modo de Análise de Clientes ---
    if (argc == 3 && std::string(argv[1]) == "-ac") {
        try {
            int n_max_clientes = std::stoi(argv[2]);
            if (n_max_clientes < 2) {
                std::cerr << "Erro: O numero maximo de clientes para analise deve ser pelo menos 2." << std::endl;
                return 1;
            }
            analiseClientes(n_max_clientes, DEFAULT_ANALYSIS_N);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro: Argumento numerico invalido para analise de clientes." << std::endl;
            return 1;
        }
        return 0;
    }

    // --- Modo de Geração de Análise ---
    if (argc == 6 && std::string(argv[1]) == "-g") {
        try {
            int numPacotesPorCliente = std::stoi(argv[2]);
            int numClientes = std::stoi(argv[3]);
            int numEventosPorPacote = std::stoi(argv[4]);
            std::string nomeArquivoSaida = argv[5];

            if (numPacotesPorCliente <= 0 || numClientes < 2 || numEventosPorPacote < 1) {
                std::cerr << "Erro: Argumentos invalidos para geracao." << std::endl;
                return 1;
            }

            std::vector<std::string> queries;
            int totalEventos = numPacotesPorCliente * numClientes * numEventosPorPacote;
            std::cout << "Gerando " << totalEventos << " queries em memoria..." << std::endl;
            gerarQueries(queries, numPacotesPorCliente, numClientes, numEventosPorPacote);
            std::cout << "Queries geradas. Executando e salvando resultados em '" << nomeArquivoSaida << "'..." << std::endl;

            std::ofstream outFile(nomeArquivoSaida);
            if (!outFile.is_open()) {
                std::cerr << "Erro: Nao foi possivel criar o arquivo de saida '" << nomeArquivoSaida << "'" << std::endl;
                return 1;
            }

            std::streambuf* coutbuf_original = std::cout.rdbuf();
            std::cout.rdbuf(outFile.rdbuf());

            Controlador controlador;
            for (const std::string& query : queries) {
                controlador.executarQuery(query);
            }

            std::cout.rdbuf(coutbuf_original);
            outFile.close();
            std::cout << "Execucao concluida. Resultados salvos em '" << nomeArquivoSaida << "'." << std::endl;

        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro: Argumentos numericos de geracao invalidos." << std::endl;
            return 1;
        }
        return 0;
    }

    // --- Modo de Execução Padrão ---
    if (argc == 2) {
        Controlador controlador;
        std::ifstream inputFile(argv[1]);
        if (!inputFile.is_open()) {
            std::cerr << "Erro: Nao foi possivel abrir o arquivo '" << argv[1] << "'" << std::endl;
            return 1;
        }
        std::string linha;
        while (std::getline(inputFile, linha)) {
            if (linha.empty()) continue;
            controlador.executarQuery(linha);
        }
        inputFile.close();
        return 0;
    }

    // --- Mensagem de Uso ---
    std::cerr << "Uso:" << std::endl;
    std::cerr << "  Modo de Execucao: " << argv[0] << " <arquivo_de_entrada.txt>" << std::endl;
    std::cerr << "  Modo de Geracao:  " << argv[0] << " -g <pacotes_por_cliente> <clientes> <eventos_por_pacote> <arquivo_saida.txt>" << std::endl;
    std::cerr << "  Analise Clientes: " << argv[0] << " -ac <max_clientes>" << std::endl;
    std::cerr << "  Analise Pacotes:  " << argv[0] << " -ap <max_pacotes_por_cliente>" << std::endl;
    std::cerr << "  Analise Eventos:  " << argv[0] << " -ae <max_eventos_por_pacote>" << std::endl;
    std::cerr << "  Analise Completa: " << argv[0] << " -a <n_max> <default_val>" << std::endl;
    return 1;
}

/**
 * @brief Realiza uma análise experimental variando o número de eventos por pacote.
 */
void analiseEventos(int n_max_eventos, int default_n) {
    std::ofstream csvFile("dadosEvento.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo dadosEvento.csv" << std::endl;
        return;
    }

    csvFile << "eventos_por_pacote,tempo_ms" << std::endl;
    std::cout << "Iniciando analise de eventos. Resultados serao salvos em dadosEvento.csv" << std::endl;

    for (int i = 1; i <= n_max_eventos; ++i) {
        std::vector<std::string> queries;
        const int numClientes = default_n;
        const int numPacotesPorCliente = default_n;

        gerarQueries(queries, numPacotesPorCliente, numClientes, i);

        auto start = std::chrono::high_resolution_clock::now();
        Controlador controlador;
        std::streambuf* coutbuf_original = std::cout.rdbuf();
        std::stringstream ss_null;
        std::cout.rdbuf(ss_null.rdbuf());

        for (const std::string& query : queries) {
            controlador.executarQuery(query);
        }

        std::cout.rdbuf(coutbuf_original);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        csvFile << i << "," << duration.count() << std::endl;
        std::cout << "Analise para " << i << " eventos/pacote concluida. Tempo: " << duration.count() << " ms." << std::endl;
    }

    csvFile.close();
    std::cout << "Analise de eventos finalizada." << std::endl;
}


/**
 * @brief Realiza uma análise experimental variando o número de pacotes por cliente.
 */
void analisePacotes(int n_max_pacotes, int default_n) {
    std::ofstream csvFile("dadosPacote.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo dadosPacote.csv" << std::endl;
        return;
    }

    csvFile << "pacotes_por_cliente,tempo_ms" << std::endl;
    std::cout << "Iniciando analise de pacotes. Resultados serao salvos em dadosPacote.csv" << std::endl;

    for (int i = 1; i <= n_max_pacotes; ++i) {
        std::vector<std::string> queries;
        const int numClientes = default_n;
        const int numEventosPorPacote = default_n;

        gerarQueries(queries, i, numClientes, numEventosPorPacote);

        auto start = std::chrono::high_resolution_clock::now();
        Controlador controlador;
        std::streambuf* coutbuf_original = std::cout.rdbuf();
        std::stringstream ss_null;
        std::cout.rdbuf(ss_null.rdbuf());

        for (const std::string& query : queries) {
            controlador.executarQuery(query);
        }

        std::cout.rdbuf(coutbuf_original);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        csvFile << i << "," << duration.count() << std::endl;
        std::cout << "Analise para " << i << " pacotes/cliente concluida. Tempo: " << duration.count() << " ms." << std::endl;
    }

    csvFile.close();
    std::cout << "Analise de pacotes finalizada." << std::endl;
}


/**
 * @brief Realiza uma análise experimental variando o número de clientes.
 */
void analiseClientes(int n_max_clientes, int default_n) {
    std::ofstream csvFile("dadosCliente.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo dadosCliente.csv" << std::endl;
        return;
    }

    csvFile << "clientes,tempo_ms" << std::endl;
    std::cout << "Iniciando analise de clientes. Resultados serao salvos em dadosCliente.csv" << std::endl;

    for (int i = 2; i <= n_max_clientes; ++i) {
        std::vector<std::string> queries;
        const int numPacotesPorCliente = default_n;
        const int numEventosPorPacote = default_n;

        gerarQueries(queries, numPacotesPorCliente, i, numEventosPorPacote);

        auto start = std::chrono::high_resolution_clock::now();
        Controlador controlador;
        std::streambuf* coutbuf_original = std::cout.rdbuf();
        std::stringstream ss_null;
        std::cout.rdbuf(ss_null.rdbuf());

        for (const std::string& query : queries) {
            controlador.executarQuery(query);
        }

        std::cout.rdbuf(coutbuf_original);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        csvFile << i << "," << duration.count() << std::endl;
        std::cout << "Analise para " << i << " clientes concluida. Tempo: " << duration.count() << " ms." << std::endl;
    }

    csvFile.close();
    std::cout << "Analise de clientes finalizada." << std::endl;
}


/**
 * @brief Gera um conjunto de queries aleatórias e as armazena em um vetor.
 */
void gerarQueries(std::vector<std::string>& queries, int numPacotesPorCliente, int numClientes, int numEventosPorPacote) {
    srand(time(0));

    const int totalPacotes = numPacotesPorCliente * numClientes;
    const int totalEventos = totalPacotes * numEventosPorPacote;
    queries.clear();
    queries.reserve(totalEventos);

    std::vector<std::string> nomesClientes;
    for (int i = 0; i < numClientes; ++i) {
        nomesClientes.push_back("Cliente" + std::to_string(i));
    }

    long long hora = 1;
    int idPacoteAtual = 1;

    // Fase 1: Registrar 'numPacotesPorCliente' para cada cliente.
    for (int c = 0; c < numClientes; ++c) {
        for (int p = 0; p < numPacotesPorCliente; ++p) {
            Evento evento;
            evento.setHora(hora++);
            evento.setTipo(Evento::RG);
            evento.setIdPacote(idPacoteAtual++);

            int idxOrigem = c;
            int idxDestino;
            do {
                idxDestino = rand() % numClientes;
            } while (idxDestino == idxOrigem);

            evento.setClienteOrigem(nomesClientes[idxOrigem]);
            evento.setClienteDestino(nomesClientes[idxDestino]);
            evento.setArmazemOrigem(rand() % 10 + 1);
            evento.setArmazemDestino(rand() % 10 + 1);

            queries.push_back(evento.eventoPraString());
        }
    }

    // Fase 2: Gerar eventos operacionais aleatórios para cada pacote
    if (numEventosPorPacote > 1) {
        for (int id = 1; id <= totalPacotes; ++id) {
            for (int e = 0; e < numEventosPorPacote - 1; ++e) {
                Evento evento;
                evento.setHora(hora++);
                evento.setIdPacote(id);

                int tipoSorteado = rand() % 5;
                switch (tipoSorteado) {
                    case 0: evento.setTipo(Evento::AR); evento.setArmazemDestino(rand() % 10 + 1); evento.setSecaoDestino(rand() % 100 + 1); break;
                    case 1: evento.setTipo(Evento::RM); evento.setArmazemDestino(rand() % 10 + 1); evento.setSecaoDestino(rand() % 100 + 1); break;
                    case 2: evento.setTipo(Evento::UR); evento.setArmazemDestino(rand() % 10 + 1); evento.setSecaoDestino(rand() % 100 + 1); break;
                    case 3: evento.setTipo(Evento::TR); evento.setArmazemOrigem(rand() % 10 + 1); evento.setArmazemDestino(rand() % 10 + 1); break;
                    case 4: evento.setTipo(Evento::EN); evento.setArmazemDestino(rand() % 10 + 1); break;
                }
                queries.push_back(evento.eventoPraString());
            }
        }
    }
}
