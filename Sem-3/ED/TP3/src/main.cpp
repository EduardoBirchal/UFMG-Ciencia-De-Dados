#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "controlador.hpp"

/**
 * @brief Função principal que lê queries de um arquivo e as executa.
 *
 * O programa espera receber o caminho para um arquivo de texto como
 * argumento de linha de comando. Cada linha nesse arquivo é tratada
 * como uma query a ser executada pelo Controlador.
 *
 * @param argc Número de argumentos da linha de comando.
 * @param argv Array de argumentos da linha de comando.
 * @return int Retorna 0 em caso de sucesso, 1 em caso de erro.
 */
int main(int argc, char* argv[]) {
    // Verifica se o nome do arquivo foi fornecido como argumento na linha de comando.
    if (argc < 2) {
        std::cerr << "Erro: Por favor, forneca o nome do arquivo de entrada como argumento." << std::endl;
        std::cerr << "Uso: " << argv[0] << " <arquivo_de_entrada.txt>" << std::endl;
        return 1; // Retorna um código de erro
    }

    // Cria o controlador principal do sistema.
    Controlador controlador;

    // Tenta abrir o arquivo de entrada fornecido.
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo '" << argv[1] << "'" << std::endl;
        return 1; // Retorna um código de erro
    }

    std::string linha;
    // Loop para ler o arquivo linha por linha.
    while (std::getline(inputFile, linha)) {
        // Ignora linhas vazias que possam existir no arquivo.
        if (linha.empty()) {
            continue;
        }

        try {
            // Executa a query contida na linha lida.
            controlador.executarQuery(linha);
        } catch (const std::exception& e) {
            // Captura e reporta exceções que podem ocorrer durante o processamento,
            // como formatos de linha inválidos, e continua para a próxima linha.
            std::cerr << "Aviso: Erro ao processar a linha: '" << linha << "'. Detalhes: " << e.what() << std::endl;
        }
    }

    // Fecha o arquivo após a leitura.
    inputFile.close();

    return 0; // Indica que o programa terminou com sucesso.
}
