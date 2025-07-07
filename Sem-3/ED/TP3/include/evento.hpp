#ifndef EVENTO_HPP
#define EVENTO_HPP

#include <string>

/**
 * @class Evento
 * @brief Representa um evento de logística
 */
class Evento {
public:
    /**
     * @enum TipoEvento
     * @brief Enumeração dos tipos de eventos possíveis
     */
    enum TipoEvento {
        RG, // Registro
        AR, // Armazenamento
        RM, // Remoção
        UR, // Rearmazenamento
        TR, // Transferência
        EN  // Entrega
    };

private:
    // Atributos
    int hora;
    TipoEvento tipo;
    int idPacote;
    std::string clienteOrigem;
    std::string clienteDestino;
    int armazemOrigem;
    int armazemDestino;
    int secaoDestino;

    /**
     * @brief Converte um enum TipoEvento para sua representação em string
     * @param tipo O tipo de evento
     * @return A string correspondente (ex: "RG", "AR")
     */
    static std::string tipoPraString(TipoEvento tipo);


public:
    /**
     * @brief Construtor padrão
     */
    Evento();

    /**
     * @brief Construtor de cópia
     * @param outro O objeto Evento a ser copiado
     */
    Evento(const Evento& outro);

    /**
     * @brief Converte uma string para o enum TipoEvento correspondente
     * @param str A string a ser convertida (ex: "RG", "AR")
     * @return O valor de TipoEvento correspondente
     */
    static TipoEvento stringPraTipo(const std::string& str);

    /**
     * @brief Cria um objeto Evento a partir de uma string formatada
     * @param linha A string contendo os dados do evento
     * @return Um objeto Evento preenchido
     */
    static Evento stringPraEvento(const std::string& linha);

    /**
     * @brief Converte os dados do evento para uma string formatada
     * @return Uma string representando o evento no formato de entrada
     */
    std::string eventoPraString() const;

    // Getters
    int getHora() const;
    TipoEvento getTipo() const;
    int getIdPacote() const;
    std::string getClienteOrigem() const;
    std::string getClienteDestino() const;
    int getArmazemOrigem() const;
    int getArmazemDestino() const;
    int getSecaoDestino() const;

    // Setters
    void setHora(int hora);
    void setTipo(TipoEvento tipo);
    void setIdPacote(int idPacote);
    void setClienteOrigem(const std::string& clienteOrigem);
    void setClienteDestino(const std::string& clienteDestino);
    void setArmazemOrigem(int armazemOrigem);
    void setArmazemDestino(int armazemDestino);
    void setSecaoDestino(int secaoDestino);
};

#endif // EVENTO_HPP
