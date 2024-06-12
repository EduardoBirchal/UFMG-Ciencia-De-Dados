#include <stdio.h>
#include <stdlib.h>

#define TIPO_VETOR double

int inicializaVetor (TIPO_VETOR** vet) {
    int tamanho;

    scanf("%d", &tamanho);
    *vet = (TIPO_VETOR *) malloc(sizeof(TIPO_VETOR) * tamanho);

    for (int i = 0; i < tamanho; i++) {
        scanf("%lf", &( (*vet)[i] ));
    }

    return tamanho;
}

void media (double *vet, int n, int *i) {
    int soma = 0, indiceMaisProximo = 0, menorDiferenca = 2147483647, media;

    for (int j = 0; j < n; j++) {
        soma += vet[j];
    }

    media = soma/n;

    for (int j = 0; j < n; j++) {
        int diferencaAtual = abs(vet[j] - media);

        if (diferencaAtual < menorDiferenca) {
            indiceMaisProximo = j;
            menorDiferenca = diferencaAtual;
        }
    }

    printf("%d", indiceMaisProximo);
    *i = indiceMaisProximo;
}

int main (int argc, char **argv) {
    double *vetor;
    int indice, tamanho = inicializaVetor (&vetor);

    media (vetor, tamanho, &indice);

    free(vetor);
    return EXIT_SUCCESS;
}