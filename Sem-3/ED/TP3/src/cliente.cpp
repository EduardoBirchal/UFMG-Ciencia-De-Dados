#include "cliente.hpp"
#include <iostream> // Para usar std::cout

/**
 * @brief Construtor
 * @param nome O nome para inicializar o cliente.
 * As listas não gerenciam a memória dos ponteiros.
 */
Cliente::Cliente(const std::string& nome) 
    : nome(nome), pacotesEnviar(false), pacotesReceber(false) {
}

/**
 * @brief Destrutor
 */
Cliente::~Cliente() {
}

bool Cliente::operator<(const Cliente& outro) const {
    return this->nome < outro.nome;
}

bool Cliente::operator==(const Cliente& outro) const {
    return this->nome == outro.nome;
}

// --- Lógica do Radix Sort ---

/**
 * @brief Obtém o maior valor de chave no array
 */
long long Cliente::getMax(parOrdenado<Pacote*, long long> arr[], int n) {
    if (n <= 0) return 0;
    long long maxVal = arr[0].segundo;
    for (int i = 1; i < n; i++) {
        if (arr[i].segundo > maxVal) {
            maxVal = arr[i].segundo;
        }
    }
    return maxVal;
}

/**
 * @brief Realiza o Counting Sort para um dígito específico (exp)
 */
void Cliente::countingSort(parOrdenado<Pacote*, long long> arr[], int n, long long exp) {
    if (n <= 0) return;
    // Aloca um array dinâmico em vez de usar std::vector
    parOrdenado<Pacote*, long long>* output = new parOrdenado<Pacote*, long long>[n];
    int count[10] = {0};

    // Conta a ocorrência de cada dígito
    for (int i = 0; i < n; i++) {
        count[(arr[i].segundo / exp) % 10]++;
    }

    // Calcula as posições cumulativas
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // Constrói o array de saída
    for (int i = n - 1; i >= 0; i--) {
        output[count[(arr[i].segundo / exp) % 10] - 1] = arr[i];
        count[(arr[i].segundo / exp) % 10]--;
    }

    // Copia o array de saída para o array original
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }

    // Libera a memória alocada para o array temporário
    delete[] output;
}

/**
 * @brief Implementação do Radix Sort
 */
void Cliente::radixSort(parOrdenado<Pacote*, long long> arr[], int n) {
    if (n <= 0) return;
    long long m = getMax(arr, n);
    for (long long exp = 1; m / exp > 0; exp *= 10) {
        countingSort(arr, n, exp);
    }
}

/**
 * @brief Imprime informações sobre os pacotes do cliente, ordenadas por chave
 */
void Cliente::printInformacao() const {
    int tamanhoEnviar = this->pacotesEnviar.getSize();
    int tamanhoReceber = this->pacotesReceber.getSize();
    int totalPacotes = tamanhoEnviar + tamanhoReceber;

    if (totalPacotes == 0) {
        std::cout << 0 << std::endl;
        return;
    }

    // Aloca arrays dinâmicos para armazenar os pares (pacote, chave)
    parOrdenado<Pacote*, long long>* primeirosEventos = new parOrdenado<Pacote*, long long>[totalPacotes];
    parOrdenado<Pacote*, long long>* ultimosEventos = new parOrdenado<Pacote*, long long>[totalPacotes];

    int index = 0;
    // Preenche os arrays com os dados dos pacotes a enviar
    for (int i = 0; i < tamanhoEnviar; ++i) {
        Pacote* pacote = this->pacotesEnviar.readIndex(i);
        if (pacote && !pacote->getHistoricoEventos().isEmpty()) {
            primeirosEventos[index] = {pacote, calcularChave(pacote, pacote->getPrimeiroEvento())};
            ultimosEventos[index] = {pacote, calcularChave(pacote, pacote->getUltimoEvento())};
            index++;
        }
    }
    // Preenche os arrays com os dados dos pacotes a receber
    for (int i = 0; i < tamanhoReceber; ++i) {
        Pacote* pacote = this->pacotesReceber.readIndex(i);
        if (pacote && !pacote->getHistoricoEventos().isEmpty()) {
            primeirosEventos[index] = {pacote, calcularChave(pacote, pacote->getPrimeiroEvento())};
            ultimosEventos[index] = {pacote, calcularChave(pacote, pacote->getUltimoEvento())};
            index++;
        }
    }
    
    int actualSize = index; // O número real de pacotes com eventos

    // Ordena os arrays usando Radix Sort
    radixSort(primeirosEventos, actualSize);
    radixSort(ultimosEventos, actualSize);

    // Imprime o número total de linhas que serão impressas
    std::cout << actualSize * 2 << std::endl;

    // Imprime os eventos ordenados
    for (int i = 0; i < actualSize; ++i) {
        std::cout << primeirosEventos[i].primeiro->getPrimeiroEvento().eventoPraString() << std::endl;
    }
    for (int i = 0; i < actualSize; ++i) {
        std::cout << ultimosEventos[i].primeiro->getUltimoEvento().eventoPraString() << std::endl;
    }

    // Libera a memória alocada para os arrays dinâmicos
    delete[] primeirosEventos;
    delete[] ultimosEventos;
}

/**
 * @brief Calcula uma chave única para um par pacote-evento
 */
long long Cliente::calcularChave(Pacote* pacote, const Evento& evento) {
    if (pacote == nullptr) {
        return 0;
    }
    long long hora_ll = evento.getHora();
    long long id_ll = pacote->getId();

    return id_ll + (hora_ll * 1000);
}

// --- Implementação dos Getters ---

std::string Cliente::getNome() const {
    return this->nome;
}

linkedList<Pacote*>& Cliente::getPacotesEnviar() {
    return this->pacotesEnviar;
}

linkedList<Pacote*>& Cliente::getPacotesReceber() {
    return this->pacotesReceber;
}

// --- Implementação dos Setters ---

void Cliente::setNome(const std::string& nome) {
    this->nome = nome;
}

// --- Implementação dos Métodos de Manipulação ---

void Cliente::adicionarPacoteEnviar(Pacote* pacote) {
    this->pacotesEnviar.addToTail(pacote);
}

void Cliente::adicionarPacoteReceber(Pacote* pacote) {
    this->pacotesReceber.addToTail(pacote);
}
