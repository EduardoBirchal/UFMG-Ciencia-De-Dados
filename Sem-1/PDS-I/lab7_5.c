#include <stdio.h>
#include <stdlib.h>

#define MAX_STR 100

int main (int argc, char **argv) {
    char str[MAX_STR], alvo;
    int numEncontrados = 0;

    fgets(str, MAX_STR, stdin);
    scanf("%c", &alvo);

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == alvo)
            numEncontrados++;
    }

    printf("%d\n", numEncontrados);

    return EXIT_SUCCESS;
}