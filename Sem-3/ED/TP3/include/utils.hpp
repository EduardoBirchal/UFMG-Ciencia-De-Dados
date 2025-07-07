#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <iomanip>

/**
 * @struct NodeDeleter
 * @brief Helper para deleção condicional de dados em nós de estruturas de dados
 * A especialização para ponteiros deleta o dado, enquanto a versão
 * genérica não faz nada para tipos de valor
 */
// Template genérico para não-ponteiros
template<typename T>
struct NodeDeleter {
    // Não faz nada
    static void destroy(T) { }
};

// Especialização para ponteiros
template<typename T>
struct NodeDeleter<T*> {
    // Deleta o ponteiro
    static void destroy(T* ptr) {
        delete ptr;
    }
};

/**
 * @brief Formata um número inteiro com um número específico de dígitos, preenchendo com zeros
 * @param n O número a ser formatado
 * @param numDigitos O número total de dígitos que a string deve ter
 * @return A string formatada
 */
inline std::string formatarInt(int n, int numDigitos) {
    std::stringstream ss;
    ss << std::setw(numDigitos) << std::setfill('0') << n;
    return ss.str();
}

/**
 * @struct parOrdenado
 * @brief Estrutura genérica para armazenar um par de valores de tipos arbitrários
 *
 * @tparam T1 O tipo do primeiro elemento
 * @tparam T2 O tipo do segundo elemento
 */
template <typename T1, typename T2>
struct parOrdenado {
    T1 primeiro;
    T2 segundo;

    // Construtor padrão
    parOrdenado() = default;

    // Construtor para inicializar os membros
    parOrdenado(const T1& p, const T2& s) : primeiro(p), segundo(s) {}
};

#endif // UTILS_HPP
