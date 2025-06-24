#include "rede.hpp"
#include "queue.hpp" // Necessário para o algoritmo BFS
#include "stack.hpp" // Necessário para reconstruir o caminho
#include <stdexcept>

// Construtor
Rede::Rede(int max_armazens, int max_transporte) {
    this->maxArmazens = max_armazens;
    this->numArmazensAtual = 0;
    this->maxTransporte = max_transporte;

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

// Método auxiliar privado para encontrar o índice de um armazém pelo ID
int Rede::findIndex(int id_armazem) const {
    for (int i = 0; i < numArmazensAtual; ++i) {
        if (armazens[i] && armazens[i]->getID() == id_armazem) return i;
    }
    return -1; // Não encontrado
}

// Adiciona um armazém na próxima posição livre da rede
void Rede::addArmazem(Armazem* armazem) {
    if (numArmazensAtual >= maxArmazens) {
        delete armazem;
        throw std::overflow_error("A capacidade maxima de armazens na rede foi atingida");
    }
    if (findIndex(armazem->getID()) != -1) {
        delete armazem;
        throw std::invalid_argument("Ja existe um armazem com este ID na rede");
    }
    this->armazens[numArmazensAtual] = armazem;
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
        this->matrizAdjacencia[index_destino][index_origem] = 1;
    } else {
        throw std::invalid_argument("ID de origem ou destino nao encontrado na rede para criar conexao");
    }
}

// Retorna um ponteiro para um armazém pelo seu ID
Armazem* Rede::getArmazem(int id) {
    int index = findIndex(id);
    return (index != -1) ? this->armazens[index] : nullptr;
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

    // Estruturas de dados para o BFS
    Queue<int> fila_bfs;
    bool* visitados = new bool[numArmazensAtual];
    int* predecessores = new int[numArmazensAtual];

    for (int i = 0; i < numArmazensAtual; i++) {
        visitados[i] = false;
        predecessores[i] = -1;
    }

    // Inicia o BFS
    visitados[index_origem] = true;
    fila_bfs.enqueue(index_origem);

    while (!fila_bfs.isEmpty()) {
        int u = fila_bfs.dequeue();

        if (u == index_destino) break; // Caminho encontrado

        for (int v = 0; v < numArmazensAtual; ++v) {
            if (matrizAdjacencia[u][v] == 1 && !visitados[v]) {
                visitados[v] = true;
                predecessores[v] = u;
                fila_bfs.enqueue(v);
            }
        }
    }

    // Reconstrói o caminho se ele foi encontrado
    if (predecessores[index_destino] != -1) {
        stack<int> caminho_inverso;
        int atual = index_destino;
        while (atual != index_origem) {
            caminho_inverso.push(armazens[atual]->getID());
            atual = predecessores[atual];
        }

        // Limpa a rota antiga e preenche com a nova
        Queue<int>& rota_pacote = pacote->getRota();
        while (!rota_pacote.isEmpty()) rota_pacote.dequeue();
        
        while(!caminho_inverso.isEmpty()){
            rota_pacote.enqueue(caminho_inverso.pop());
        }
    }
    
    // Libera a memória alocada
    delete[] visitados;
    delete[] predecessores;
    
    // Define o próximo passo imediato do pacote
    pacote->atualizarDestinoAtual();
}

