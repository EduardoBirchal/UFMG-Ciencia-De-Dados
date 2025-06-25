#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "escalonador.hpp"
#include "rede.hpp"
#include "pacote.hpp"
#include "evento.hpp"
#include "armazem.hpp"

int main(int argc, char* argv[]) {
    // 1. Validação dos Argumentos de Linha de Comando
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.wkl>" << std::endl;
        return 1;
    }

    // 2. Abertura e Validação do Arquivo de Entrada
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo " << argv[1] << std::endl;
        return 1;
    }

    // 3. Leitura dos Parâmetros da Simulação
    int capacidade_transporte, latencia_transporte, intervalo_transportes, custo_remocao, num_armazens, num_pacotes;
    
    inputFile >> capacidade_transporte >> latencia_transporte >> intervalo_transportes >> custo_remocao >> num_armazens;

    // 4. Inicialização do Sistema com os Parâmetros Lidos
    int max_eventos = num_armazens * 1000; 

    Rede rede(num_armazens, capacidade_transporte, custo_remocao);
    Escalonador escalonador(max_eventos, 0, latencia_transporte, intervalo_transportes, &rede);

    // 5. Configuração da Rede de Armazéns
    for (int i = 0; i < num_armazens; ++i) {
        rede.addArmazem(i);
    }

    int conexao;
    for (int i = 0; i < num_armazens; ++i) {
        for (int j = 0; j < num_armazens; ++j) {
            inputFile >> conexao;
            if (conexao == 1 && j > i) {
                rede.addConexao(i, j);
            }
        }
    }
    
    inputFile >> num_pacotes;

    // 6. Leitura e Agendamento dos Pacotes
    std::string line;
    int pacote_id_counter = 0;
    std::getline(inputFile, line); // Consome a quebra de linha após o número de pacotes

    while (pacote_id_counter < num_pacotes && std::getline(inputFile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        int hora_postagem, id_entrada, id_origem, id_destino_final;
        std::string dummy;

        // CORREÇÃO: Lógica de parsing simplificada para corresponder ao formato do arquivo.
        // Formato esperado: "10 pac 1101 org 0 dst 2"
        ss >> hora_postagem >> dummy >> id_entrada >> dummy >> id_origem >> dummy >> id_destino_final;
        
        Pacote* p = new Pacote(hora_postagem, id_entrada, id_origem, id_destino_final, pacote_id_counter);
        escalonador.agendar<EventoPostagem>(p, p->getOrigem(), p->getDestinoFinal(), 0, p->getHoraPostagem());
        
        pacote_id_counter++;
    }
    
    // CHAMADA DA FUNÇÃO DE TESTE para ver o estado antes da simulação começar
    // imprimirEstadoDaRede(rede); // Descomente para depuração

    // 7. Loop Principal da Simulação
    while (!escalonador.vazio()) {
        escalonador.simularProximo();
    }

    return 0;
}
