#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 100
#define MAX_TIPO 15
#define NUM_TIPOS 5
#define INVALIDO -1
#define MODIFICADOR_EFETIVIDADE 0.2

char tipos[NUM_TIPOS][MAX_TIPO] = {
    "fogo",
    "gelo",
    "pedra",
    "elétrico",
    "água",
};

typedef struct pokemon_t {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int ataque;
    int defesa;
    int vida;
};

// Retorna o índice no vetor de tipos correspondente à string recebida. Se não houver índice correspondente, retorna INVALIDO.
int getIndiceTipo (char *str) {
    for (int i = 0; i < NUM_TIPOS; i++) {
        if (strcmp(str, tipos[i]) == 0) {
            return i;
        }
    }
    
    return INVALIDO; // Se o tipo não está no vetor, ele é inválido.
}

// Retorna o multiplicador de dano de um ataque baseado no tipo do pokemon alvo.
float calculaEfetividade (char *tipoAtaque, char *tipoAlvo) {
    int indiceAtaque = getIndiceTipo(tipoAtaque);
    int indiceAlvo = getIndiceTipo(tipoAlvo);

    if (indiceAtaque != INVALIDO && indiceAlvo != INVALIDO) {
        if (indiceAlvo == (indiceAtaque + 1) % NUM_TIPOS)   // Tipos têm vantagem contra o tipo um índice à frente deles.
            return 1.0 + MODIFICADOR_EFETIVIDADE;

        if (indiceAlvo == (indiceAtaque - 1) % NUM_TIPOS)   // Tipos têm desvantagem contra o tipo um índice atrás deles.
            return 1.0 - MODIFICADOR_EFETIVIDADE;

        else                                                // Se dois tipos não estão adjacentes sua interação é neutra.
            return 1.0;
    }
    else {
        printf("Tipo invalido detectado! (Tipo do ataque: '%s'; tipo do alvo: '%s')", tipoAtaque, tipoAlvo);

        return INVALIDO;
    }
}

int main (int argc, char **argv) {
    

    return EXIT_SUCCESS;
}