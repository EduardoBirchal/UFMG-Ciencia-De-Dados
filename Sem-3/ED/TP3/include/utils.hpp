#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <iomanip>

/**
 * @struct NodeDeleter
 * @brief Helper para deleção condicional de dados em nós de estruturas de dados.
 * A especialização para ponteiros deleta o dado, enquanto a versão
 * genérica não faz nada para tipos de valor.
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
 * @brief Formata um número inteiro com um número específico de dígitos, preenchendo com zeros.
 * @param n O número a ser formatado.
 * @param numDigitos O número total de dígitos que a string deve ter.
 * @return A string formatada.
 */
inline std::string formatarInt(int n, int numDigitos) {
    std::stringstream ss;
    ss << std::setw(numDigitos) << std::setfill('0') << n;
    return ss.str();
}

#endif // UTILS_HPP
