#include <stdio.h>
#include <stdlib.h>

#define TIPO_VETOR Evento

enum meses { jan, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec };

typedef struct data_t {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct evento_t {
    char nome[100];
    char local[100];
    Data data;
} Evento;

int inicializaVetor (TIPO_VETOR** vet) {
    int tamanho;

    scanf("%d", &tamanho);
    *vet = (TIPO_VETOR *) malloc(sizeof(TIPO_VETOR) * tamanho);

    for (int i = 0; i < tamanho; i++) {
        scanf("%s %s %d %d %d", &( (*vet)[i].nome ), &( (*vet)[i].local ), &( (*vet)[i].nome ));
    }

    return tamanho;
}

int main (int argc, char **argv) {
    

    return EXIT_SUCCESS;
}