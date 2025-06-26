#ifndef ESTATISTICAS_HPP
#define ESTATISTICAS_HPP

#include <string>
#include <vector>
#include <utility>

// Classe Singleton para rastrear estatísticas da simulação.
class Estatisticas {
public:
    static Estatisticas& instancia();
    void incrementarPostagens();
    void incrementarArmazenamentos();
    void incrementarEnviados();
    void incrementarRearmazenados();
    void incrementarEntregues();
    void incrementarRemocoes();
    int getTotalOperacoes() const;
    void reset();
    // MODIFICADO: O nome do arquivo agora é um parâmetro
    void salvarAnaliseEmCSV(const std::string& nomeArquivo, const std::string& colunaVariavel, const std::vector<std::pair<int, int>>& resultados) const;

private:
    Estatisticas() = default;
    ~Estatisticas() = default;
    Estatisticas(const Estatisticas&) = delete;
    Estatisticas& operator=(const Estatisticas&) = delete;

    int pacotesPostados = 0;
    int pacotesArmazenados = 0;
    int pacotesEnviados = 0;
    int pacotesRearmazenados = 0;
    int pacotesEntregues = 0;
    int remocoesDePacotes = 0;
};

#endif // ESTATISTICAS_HPP
