#include "rede.hpp"
#include "queue.hpp"
#include "stack.hpp"
#include "escalonador.hpp"
#include <stdexcept>

// Construtor
Rede::Rede(int max_armazens, int max_transporte, int custo_remocao) 
    : maxArmazens(max_armazens),
      numArmazensAtual(0),
      maxTransporte(max_transporte),
      custoRemocao(custo_remocao),
      escalonador(nullptr) { // Inicializa o escalonador como nulo

    this->armazens = new Armazem*[maxArmazens];
    for (int i = 0; i < maxArmazens; ++i) this->armazens[i] = nullptr;

    this->matrizAdjacencia = new int*[maxArmazens];
    for (int i = 0; i < maxArmazens; ++i) {
        this->matrizAdjacencia[i] = new int[maxArmazens];
        for (int j = 0; j < maxArmazens; ++j) {
            this->matrizAdjacencia[i][j] = 0;
        }
    }
}

// Destrutor
Rede::~Rede() {
    for (int i = 0; i < numArmazensAtual; ++i) delete this->armazens[i];
    delete[] this->armazens;

    for (int i = 0; i < maxArmazens; ++i) delete[] this->matrizAdjacencia[i];
    delete[] this->matrizAdjacencia;
}

// Define o escalonador
void Rede::setEscalonador(Escalonador* esc) {
    this->escalonador = esc;
}

// Método auxiliar privado para encontrar o índice de um armazém pelo ID
int Rede::findIndex(int id_armazem) const {
    for (int i = 0; i < numArmazensAtual; ++i) {
        if (armazens[i] && armazens[i]->getID() == id_armazem) return i;
    }
    return -1; // Não encontrado
}

// Cria e adiciona um novo armazém na rede
void Rede::addArmazem(int id_armazem) {
    if (numArmazensAtual >= maxArmazens) {
        throw std::overflow_error("A capacidade maxima de armazens na rede foi atingida");
    }
    if (findIndex(id_armazem) != -1) {
        throw std::invalid_argument("Ja existe um armazem com este ID na rede");
    }
    if (!escalonador) {
        throw std::runtime_error("Escalonador nao foi definido na rede antes de adicionar um armazem");
    }

    // Cria um novo armazém, passando o custo de remoção e o escalonador
    Armazem* novoArmazem = new Armazem(id_armazem, this->custoRemocao, this->escalonador);
    this->armazens[numArmazensAtual] = novoArmazem;
    this->numArmazensAtual++;
}

// Adiciona uma nova seção a um armazém específico
void Rede::addSecao(int id_armazem) {
    Armazem* armazem_alvo = getArmazem(id_armazem);
    if (armazem_alvo) {
        armazem_alvo->addSecao(id_armazem);
    } else {
        throw std::invalid_argument("Armazem com o ID especificado nao foi encontrado na rede");
    }
}

// Adiciona uma conexão entre dois armazéns usando seus IDs
void Rede::addConexao(int id_origem, int id_destino) {
    int index_origem = findIndex(id_origem);
    int index_destino = findIndex(id_destino);

    if (index_origem != -1 && index_destino != -1) {
        this->matrizAdjacencia[index_origem][index_destino] = 1;
        this->matrizAdjacencia[index_destino][index_origem] = 1; // Grafo não direcionado
    } else {
        throw std::invalid_argument("ID de origem ou destino nao encontrado na rede para criar conexao");
    }
}

// Retorna um ponteiro para um armazém pelo seu ID
Armazem* Rede::getArmazem(int id) {
    int index = findIndex(id);
    return (index != -1) ? this->armazens[index] : nullptr;
}

// Retorna um armazém pelo seu índice no array
Armazem* Rede::getArmazemPeloIndice(int index) const {
    if (index >= 0 && index < numArmazensAtual) {
        return armazens[index];
    }
    return nullptr;
}

// Retorna o número de armazéns na rede
int Rede::getNumArmazens() const {
    return this->numArmazensAtual;
}

// Retorna a capacidade máxima de transporte
int Rede::getMaxTransporte() const {
    return this->maxTransporte;
}


// Calcula a rota mais curta para um pacote usando BFS
void Rede::calcularRota(Pacote* pacote) {
    int id_origem = pacote->getArmazemAtual();
    int id_destino = pacote->getDestinoFinal();
    int index_origem = findIndex(id_origem);
    int index_destino = findIndex(id_destino);

    if (index_origem == -1 || index_destino == -1) {
        throw std::invalid_argument("Armazem de origem ou destino do pacote nao existe na rede");
    }

    Queue<int> fila_bfs;
    bool* visitados = new bool[numArmazensAtual]();
    int* predecessores = new int[numArmazensAtual];
    for (int i = 0; i < numArmazensAtual; i++) predecessores[i] = -1;

    visitados[index_origem] = true;
    fila_bfs.enqueue(index_origem);

    while (!fila_bfs.isEmpty()) {
        int u = fila_bfs.dequeue();
        if (u == index_destino) break;

        for (int v = 0; v < numArmazensAtual; ++v) {
            if (matrizAdjacencia[u][v] == 1 && !visitados[v]) {
                visitados[v] = true;
                predecessores[v] = u;
                fila_bfs.enqueue(v);
            }
        }
    }

    if (predecessores[index_destino] != -1) {
        stack<int> caminho_inverso;
        for (int atual = index_destino; atual != index_origem; atual = predecessores[atual]) {
            caminho_inverso.push(armazens[atual]->getID());
        }
        
        Queue<int>& rota_pacote = pacote->getRota();
        while (!rota_pacote.isEmpty()) rota_pacote.dequeue();
        while(!caminho_inverso.isEmpty()) rota_pacote.enqueue(caminho_inverso.pop());
    }
    
    delete[] visitados;
    delete[] predecessores;
    
    pacote->atualizarDestinoAtual();
}
