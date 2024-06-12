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

void soma (int *vet1, int *vet2, int *vet3, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        vet3[i] = vet1[i] + vet2[i];
        printf("%d\n", vet3[i]);
    }
}

int main (int argc, char **argv) {
    int *vet1, *vet2, *vet3;
    int tamanho;

    tamanho = inicializaVetor(&vet1);
    
    vet2 = malloc (sizeof(int) * tamanho);
    vet3 = malloc (sizeof(int) * tamanho);

    for (int i = 0; i < tamanho; i++) {
        scanf("%d", &vet2[i]);
    }

    soma(vet1, vet2, vet3, tamanho);

    free(vet1);
    free(vet2);
    free(vet3);

    return EXIT_SUCCESS;
}