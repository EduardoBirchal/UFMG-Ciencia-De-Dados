#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int* eratostenes (int n) {
    int *primos = malloc (sizeof(int) * n+1);

    for (int i = 0; i < n; i++) primos[i] = 1;

    primos[0] = primos[1] = 0;

    for (int i = 0; i < sqrt(n); i++) {
        if (primos[i] == 1) {
            // se primos[i] é primo, nenhum múltiplo de i (até n) é primo
            for (int j = i*i; i * j <= n; j++) {
                primos[j] = 0;
            }
        }
    }

    return primos;
}

int main (int argc, char **argv) {
    int n, menor, maior;

    scanf("%d", &n);

    int *primos = eratostenes(n*n);

    for (int i=0; i < n; i++) {
        if (primos[i] == 1) {
            menor = i;
            break;
        }
    }

    for (int i=n; i < n*n; i++) {
        if (primos[i] == 1) {
            maior = i;
            break;
        }
    }

    printf("%d\n%d\n", menor, maior);

    free(primos);

    return EXIT_SUCCESS;
}