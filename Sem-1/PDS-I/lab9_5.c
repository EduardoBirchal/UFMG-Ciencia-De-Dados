#include <stdio.h>
#include <stdlib.h>

#define TIPO_VETOR int

int inicializaVetor (TIPO_VETOR** vet) {
    int tamanho;

    scanf("%d", &tamanho);
    *vet = (TIPO_VETOR *) malloc(sizeof(TIPO_VETOR) * tamanho);

    for (int i = 0; i < tamanho; i++) {
        scanf("%d", &( (*vet)[i] ));
    }

    return tamanho;
}

void imprimeVetor (int *vet, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%d\n", vet[i]);
    }
}

int main (int argc, char **argv) {
    int *vetor;
    int tamanho = inicializaVetor(&vetor);

    imprimeVetor(vetor, tamanho);
    free(vetor);

    return EXIT_SUCCESS;
}