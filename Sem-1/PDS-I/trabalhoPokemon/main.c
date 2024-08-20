#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NOME 100
#define MAX_TIPO 15
#define NUM_TIPOS 5
#define INVALIDO -1

char tipos[NUM_TIPOS][MAX_TIPO] = {
    "fogo",
    "gelo",
    "pedra",
    "eletrico",
    "agua",
};

typedef struct pokemon_t {
    char nome[MAX_NOME];
    int tipo;
    float ataque;
    float defesa;
    float vida;
} Pokemon;

typedef struct player_t {
    Pokemon *time;
    int tamanhoTime;
    int pokemonAtual;
} Player;

// Retorna o índice no vetor de tipos correspondente à string recebida. Se não houver índice correspondente, retorna INVALIDO.
int getIndiceTipo (char *str) {
    for (int i = 0; i < NUM_TIPOS; i++) {
        if (strcmp(str, tipos[i]) == 0) {
            return i;
        }
    }
    printf("Tipo invalido detectado! (Tipo '%s' nao existe)", str);
    
    return INVALIDO; // Se o tipo não está no vetor, ele é inválido.
}

// Escaneia, do arquivo, os pokemons de um player.
void getPokemon (FILE *arquivo, Pokemon **time, int n) {
    char buffer[MAX_TIPO];

    for(int i = 0; i < n; i++){
        fscanf(arquivo, " %s %f %f %f %s", (*time)[i].nome, &((*time)[i].ataque), &((*time)[i].defesa), &((*time)[i].vida), buffer);
        (*time)[i].tipo = getIndiceTipo(buffer);
    }
}

// Aloca o time do player e inicializa seus valores.
void inicializaPlayer (Player *player, int tamanho) {
    player->time = (Pokemon *) malloc (sizeof(Pokemon) * tamanho);
    player->tamanhoTime = tamanho;
    player->pokemonAtual = 0;
}

// Retorna o multiplicador de dano de um ataque baseado no tipo do pokemon alvo.
float calculaEfetividade (int indiceAtaque, int indiceAlvo) {
    if (indiceAtaque - 1 < 0)
        indiceAtaque = NUM_TIPOS; // Faz o "wrap" do valor se for negativo.

    if (indiceAtaque != INVALIDO && indiceAlvo != INVALIDO) {
        if (indiceAlvo == (indiceAtaque + 1) % NUM_TIPOS)   // Tipos têm vantagem contra o tipo um índice à frente deles.
            return 1.2;

        if (indiceAlvo == indiceAtaque - 1)                 // Tipos têm desvantagem contra o tipo um índice atrás deles.
            return 0.8;

        else                                                // Se dois tipos não estão adjacentes sua interação é neutra.
            return 1.0;
    }
}

// Calcula o dano causado por um ataque.
float getDanoAtaque (Pokemon atacante, Pokemon alvo) {
    float efetividade = calculaEfetividade(atacante.tipo, alvo.tipo);
    float ataqueAjustado = atacante.ataque * efetividade;

    float dano = ataqueAjustado - alvo.defesa;

    return dano > 0.0 ? dano : 1.0; // Operador ternário - retorna 1 se o dano não for maior que 0, e retorna o dano se for.
}

// Faz um pokemon atacar o outro, e retorna se o alvo morreu.
bool atacar (Pokemon *atacante, Pokemon *alvo) {
    float dano = getDanoAtaque (*atacante, *alvo);
    alvo->vida -= dano;

    return alvo->vida > 0.0 ? false : true;
}

// Faz um player atacar o outro com seu pokemon atual, atualizando o pokemonAtual do alvo e imprimindo o resultado se o pokemon do alvo morrer.
void turno (Player *playerAtacante, Player *playerAlvo) {
    Pokemon *pokemonAtacante = &(playerAtacante->time[playerAtacante->pokemonAtual]);
    Pokemon *pokemonAlvo = &(playerAlvo->time[playerAlvo->pokemonAtual]);

    if (atacar (pokemonAtacante, pokemonAlvo) == true) { // Se o alvo morreu
        playerAlvo->pokemonAtual += 1;
        printf("%s venceu %s\n", pokemonAtacante->nome, pokemonAlvo->nome);
    }
}

// Imprime os pokemons de um jogador.
void imprimePokemon (Player player, int i) {
    for (i; i < player.tamanhoTime; i++) {
        printf("%s\n", player.time[i].nome);
    }
}

// Imprime o vencedor e o perdedor de uma batalha.
void imprimeResultados (Player vencedor, Player perdedor) {
    printf("Pokemon sobreviventes:\n");
    imprimePokemon(vencedor, vencedor.pokemonAtual);
    printf("Pokemon derrotados:\n");
    imprimePokemon(perdedor, 0);
}

// Loop de batalha.
void batalha (Player *player1, Player *player2) {
    while (true) {
        turno (player1, player2);
        if (player2->pokemonAtual >= player2->tamanhoTime) {
            printf("Jogador 1 venceu\n");
            imprimeResultados(*player1, *player2);

            break;
        }

        turno (player2, player1);
        if (player1->pokemonAtual >= player1->tamanhoTime) {
            printf("Jogador 1 venceu\n");
            imprimeResultados(*player2, *player1);

            break;
        }
    }
}

int main (int argc, char **argv) {
    Player player1, player2;
    FILE *arquivo;
    int tamanho1, tamanho2;

    arquivo = fopen("pokemon.txt", "r");
    fscanf(arquivo, " %d %d", &tamanho1, &tamanho2);

    inicializaPlayer(&player1, tamanho1);
    inicializaPlayer(&player2, tamanho2);

    getPokemon(arquivo, &(player1.time), tamanho1);
    getPokemon(arquivo, &(player2.time), tamanho2);

    batalha(&player1, &player2);

    free(player1.time);
    free(player2.time);

    return EXIT_SUCCESS;
}