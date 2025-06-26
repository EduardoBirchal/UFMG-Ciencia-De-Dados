#include "estatisticas.hpp"
#include <fstream>
#include <iostream>

Estatisticas& Estatisticas::instancia() {
    static Estatisticas instanciaUnica;
    return instanciaUnica;
}

void Estatisticas::incrementarPostagens() { pacotesPostados++; }
void Estatisticas::incrementarArmazenamentos() { pacotesArmazenados++; }
void Estatisticas::incrementarEnviados() { pacotesEnviados++; }
void Estatisticas::incrementarRearmazenados() { pacotesRearmazenados++; }
void Estatisticas::incrementarEntregues() { pacotesEntregues++; }
void Estatisticas::incrementarRemocoes() { remocoesDePacotes++; }

int Estatisticas::getTotalOperacoes() const {
    return pacotesPostados +
           pacotesArmazenados +
           pacotesEnviados +
           pacotesRearmazenados +
           pacotesEntregues +
           remocoesDePacotes;
}

void Estatisticas::reset() {
    pacotesPostados = 0;
    pacotesArmazenados = 0;
    pacotesEnviados = 0;
    pacotesRearmazenados = 0;
    pacotesEntregues = 0;
    remocoesDePacotes = 0;
}

// CORREÇÃO: A definição da função agora corresponde exatamente à declaração no .hpp,
// aceitando os três parâmetros necessários.
void Estatisticas::salvarAnaliseEmCSV(const std::string& nomeArquivo, const std::string& colunaVariavel, const std::vector<std::pair<int, int>>& resultados) const {
    std::ofstream arquivo(nomeArquivo);

    if (!arquivo.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo de analise " << nomeArquivo << std::endl;
        return;
    }

    arquivo << colunaVariavel << ",Operacoes Totais\n";

    for (const auto& resultado : resultados) {
        arquivo << resultado.first << "," << resultado.second << "\n";
    }

    arquivo.close();
    std::cout << "Analise de complexidade salva com sucesso em " << nomeArquivo << std::endl;
}
