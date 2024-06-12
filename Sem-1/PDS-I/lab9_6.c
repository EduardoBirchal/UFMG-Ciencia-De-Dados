#include <stdio.h>
#include <stdlib.h>

void inicializaColuna (int **matriz, int tamanho, int pos) {
    matriz[pos] = malloc(sizeof(int) * tamanho);

    for (int i = 0; i < tamanho; i++) {
        scanf("%d", &matriz[pos][i]);
    }
}

void imprimeVetor (int *vet, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%d\n", vet[i]);
    }
}

int main (int argc, char **argv) {
    int **matriz;
    int x, y;

    scanf("%d\n%d", &y, &x);

    matriz = (int**) malloc (sizeof(int*) * y);

    for (int i = 0; i < y; i++) {
        inicializaColuna (matriz, x, i);
    }

    for (int i = 0; i < y; i++) {
        imprimeVetor(matriz[i], x);
    }

    for (int i = 0; i < y; i++) {
        free(matriz[i]);
    }

    free(matriz);

    return EXIT_SUCCESS;
}