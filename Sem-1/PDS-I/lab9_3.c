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
        printf("%d ", vet[i]);
    }

    printf("\n");
}

void multiplica (int *vet, int tamanho, double valor) {
    for (int i = 0; i < tamanho; i++) {
        vet[i] = (int) vet[i] * valor;
    }
}

int main(int argc, char **argv)
{
    int *vetor;
    int valor, tamanho = inicializaVetor (&vetor);

    scanf("%d", &valor);

    imprimeVetor(vetor, tamanho);
    multiplica(vetor, tamanho, valor);
    imprimeVetor(vetor, tamanho);
    multiplica(vetor, tamanho, (double) 1.0/valor);
    imprimeVetor(vetor, tamanho);

    free(vetor);

    return EXIT_SUCCESS;
}