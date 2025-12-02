#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <list>

// uso do namespace padrao
using namespace std;

// verifica se o bit na posicao i esta ligado
bool test_bit(int mascara, int i) {
    return (mascara & (1 << i)) != 0;
}

// conta quantos bits estao ligados
int count_bits(int mascara) {
    // contador de bits
    int contador = 0;
    
    // enquanto houver bits ligados
    while (mascara > 0) {
        // verifica o ultimo bit
        if (mascara & 1) {
            contador++;
        }
        // desloca para a direita
        mascara >>= 1;
    }
    return contador;
}

// variaveis globais para o algoritmo meet-in-the-middle
int n; // numero total de vertices
int n_left;
int n_right;
long long global_max_size = -1;
long long best_mask_global = 0;

// vetor de adjacencia original (global)
vector<long long> adj;

// vetores de adjacencia otimizados (pre-processados)
// adj_right[i]: vizinhos do vertice i (da metade direita) dentro da propria direita
vector<int> adj_right; 
// adj_left_to_right[i]: vizinhos do vertice i (da metade esquerda) que estao na direita
vector<int> adj_left_to_right;

// tabela sos dp armazenando {tamanho, mascara_direita}
vector<pair<int, int>> sos_dp;

// mascara com todos os bits da direita ligados
int mask_right_all;

// funcao para ler a entrada e construir o grafo
void read_graph() {
    int m;
    // le numero de vertices e arestas
    if (!(cin >> n >> m)) return;

    // calcula tamanhos das metades
    n_left = n / 2;
    n_right = n - n_left;
    
    // inicializa vetor de adjacencia com 0
    adj.assign(n, 0);

    // le as arestas
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;

        // grafo nao direcionado
        // usa 1LL para garantir shift em 64 bits
        adj[u] |= (1LL << v);
        adj[v] |= (1LL << u);
    }
}

// prepara vetores auxiliares de adjacencia para evitar shifts repetidos
void prepare_adjacencies() {
    // redimensiona os vetores
    adj_right.assign(n_right, 0);
    adj_left_to_right.assign(n_left, 0);

    // preenche adj_right (conexoes internas da direita)
    // para cada vertice i da direita (mapeado de 0 a n_right-1)
    for (int i = 0; i < n_right; ++i) {
        // o vertice original e (n_left + i)
        // pegamos seus vizinhos e deslocamos para a direita por n_left bits
        // assim, os vizinhos da direita ficam nos bits 0 a n_right-1
        adj_right[i] = (int)(adj[n_left + i] >> n_left);
    }

    // preenche adj_left_to_right (conexoes da esquerda para a direita)
    // para cada vertice i da esquerda
    for (int i = 0; i < n_left; ++i) {
        // pega os vizinhos de i e desloca para obter apenas a parte que toca a direita
        adj_left_to_right[i] = (int)(adj[i] >> n_left);
    }
}

// constroi e preenche a tabela sos dp para a metade direita
void build_sos_dp() {
    // tamanho da tabela e 2 elevado a n_right
    int size_right = 1 << n_right;
    sos_dp.assign(size_right, {0, 0});
    mask_right_all = size_right - 1;

    // nota: adj_right ja deve ter sido preenchido por prepare_adjacencies()

    // passo 1: inicializacao da base da dp
    // verifica quais mascaras sao conjuntos independentes validos por si so
    for (int mask = 0; mask < size_right; ++mask) {
        bool is_independent = true;
        
        // verifica conflitos internos na mascara
        for (int i = 0; i < n_right; ++i) {
            if (test_bit(mask, i)) {
                // usa o vetor otimizado adj_right
                if (adj_right[i] & mask) {
                    is_independent = false;
                    break;
                }
            }
        }
        
        // se valido, armazena seu tamanho e a propria mascara
        if (is_independent) {
            sos_dp[mask] = {count_bits(mask), mask};
        } else {
            sos_dp[mask] = {0, 0};
        }
    }

    // passo 2: propagacao SOS DP
    // para cada bit, propaga o melhor resultado para as supermascaras
    for (int i = 0; i < n_right; ++i) {
        for (int mask = 0; mask < size_right; ++mask) {
            // se o bit i esta ligado na mascara atual
            if (test_bit(mask, i)) {
                // prev: melhor subconjunto SEM o bit i (i.e. mascara com bit i desligado)
                pair<int, int> prev = sos_dp[mask ^ (1 << i)];
                // curr: melhor subconjunto COM o bit i permitido (valor atual)
                pair<int, int> curr = sos_dp[mask];
                
                // logica de atualizacao com desempate lexicografico
                if (prev.first > curr.first) {
                    // se a opcao sem i tem tamanho maior, pegamos ela
                    sos_dp[mask] = prev;
                } else if (prev.first == curr.first) {
                    // se tamanhos sao iguais, desempate lexicografico
                    // queremos o conjunto que tem o menor vertice (bit mais baixo ligado)
                    int m_prev = prev.second;
                    int m_curr = curr.second;
                    
                    if (m_prev != m_curr) {
                        // encontra o bit mais baixo onde as mascaras diferem
                        int diff = m_prev ^ m_curr;
                        int lowest_diff_bit = diff & -diff;
                        
                        // se prev tem esse bit ligado, ele e lexicograficamente menor
                        // exemplo: prev={0,2} (101), curr={1,2} (110). diff=011. low=001 (bit 0)
                        // prev tem bit 0. prev vence
                        if (m_prev & lowest_diff_bit) {
                            sos_dp[mask] = prev;
                        }
                        // senao, mantemos curr
                    }
                }
            }
        }
    }
}

// funcao de teste removida para entrega final ou mantida se necessario depuracao
// a implementacao original usava min() que requer <algorithm>
// se precisar testar substitua por logica manual

// funcao recursiva para processar a metade esquerda
void solve_left(int idx, int current_mask, int forbidden_in_right) {
    // caso base quando processamos todos os vertices da esquerda
    if (idx == n_left) {
        // calcula quais vertices da direita sao permitidos
        // inverte os proibidos e mantem apenas os bits validos da direita
        int allowed_in_right = mask_right_all & (~forbidden_in_right);
        
        // busca a melhor solucao pre-calculada na direita
        // sos_dp deve conter {tamanho, melhor_mascara}
        pair<int, int> best_right = sos_dp[allowed_in_right];
        
        // calcula tamanho total do conjunto independente
        int total_size = count_bits(current_mask) + best_right.first;
        
        // atualiza se encontrou um conjunto maior
        // nao atualiza em caso de empate para manter o lexicograficamente menor
        // pois a recursao prioriza a inclusao de vertices menores (idx 0, 1...)
        if (total_size > global_max_size) {
            global_max_size = total_size;
            // reconstroi a mascara global combinando esquerda e direita
            // a parte direita e deslocada para suas posicoes originais
            best_mask_global = (long long)current_mask | ((long long)best_right.second << n_left);
        }
        return;
    }

    // ramo 1: tenta incluir o vertice idx
    // priorizamos incluir para garantir menor lexicografico
    // verifica se idx nao tem conflito com vertices ja escolhidos na esquerda
    if ((adj[idx] & current_mask) == 0) {
        // calcula novas restricoes para a direita
        // usa vetor otimizado: adj_left_to_right[idx] ao inves de shift
        int new_forbidden = forbidden_in_right | adj_left_to_right[idx];
        
        // chama recursao incluindo idx na mascara atual
        solve_left(idx + 1, current_mask | (1 << idx), new_forbidden);
    }

    // ramo 2: nao inclui o vertice idx
    solve_left(idx + 1, current_mask, forbidden_in_right);
}

int main() {
    // passo 1: ler o grafo
    read_graph();

    // verificacao de seguranca para n=0
    if (n == 0) {
        cout << "0" << endl;
        return 0;
    }

    // passo 1.5: preparar adjacencias otimizadas
    prepare_adjacencies();

    // passo 2: pre-processar a metade direita (sos dp)
    build_sos_dp();

    // passo 3: processar a metade esquerda e fazer o merge
    solve_left(0, 0, 0);

    // passo 4: exibir o resultado
    // exibe o tamanho maximo
    cout << global_max_size << endl;

    // exibe os vertices do conjunto independente maximo
    bool first = true;
    for (int i = 0; i < n; ++i) {
        if ((best_mask_global >> i) & 1) {
            if (!first) cout << " ";
            cout << (i);
            first = false;
        }
    }
    cout << endl;

    return 0;
}