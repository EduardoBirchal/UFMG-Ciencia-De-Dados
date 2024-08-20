#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PESSOAS 4
#define MAX_INPUT 100
#define MAX_NOME 50

typedef struct t_pessoa {
    char nome[MAX_NOME];
    unsigned int idade;
    float salario;
} pessoa;

void removeChar (char* str, int index) {
    int i;

    for (i = index; str[i] != '\0'; i++) {
        str[i] = str[i + 1];
    }
}

void removeNewlines (char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') removeChar (str, i);
    }
}

int procurarNome (int indice, char *nomeAlvo, pessoa *array) {
    for (int i=0; i < indice; i++) {
        if (strcmp(array[i].nome, nomeAlvo) == 0) {
            return i;
        }
    }

    return -1;
}

void mostrar (int indice, char *nomeAlvo, pessoa *array) {
    int indiceEncontrado = procurarNome(indice, nomeAlvo, array);

    if (indiceEncontrado != -1) {
        printf("Registro %s %d %.2f", array[indiceEncontrado].nome, array[indiceEncontrado].idade, array[indiceEncontrado].salario);
    }
    else {
        printf("Registro ausente\n");
    }
}

int inserir (int indice, pessoa *array, char *nomeNovo, unsigned int idadeNova, float salarioNovo) {
    if (indice < MAX_PESSOAS) {
        pessoa novaPessoa = (pessoa) {"[PLACEHOLDER]", idadeNova, salarioNovo};
        strcpy(novaPessoa.nome, nomeNovo);

        array[indice] = novaPessoa;

        mostrar(indice + 1, nomeNovo, array);
        printf(" inserido\n");

        return indice + 1;
    }
    else {
        printf("Espaço insuficiente\n");
        return indice;
    }
}

int main (int argc, char **argv) {
    int pessoaAtual = 0;
    pessoa pessoas[MAX_PESSOAS];
    char input[MAX_INPUT];

    while (1) {
        if (!fgets(input, MAX_INPUT, stdin))
            fgets(input, MAX_INPUT, stdin);

        removeNewlines(input);

        if (strcmp(input, "inserir") == 0) {
            char nomeNovo[MAX_NOME];
            unsigned int idadeNova;
            float salarioNovo;

            scanf("%s %u %f", &nomeNovo, &idadeNova, &salarioNovo);

            pessoaAtual = inserir(pessoaAtual, pessoas, nomeNovo, idadeNova, salarioNovo);
        }
        else if (strcmp(input, "mostrar") == 0) {
            char nomeAlvo[MAX_NOME];

            fgets(nomeAlvo, MAX_NOME, stdin);
            removeNewlines(nomeAlvo);

            mostrar(pessoaAtual, nomeAlvo, pessoas);
            printf("\n");
        }
        else {
            printf("Terminando programa...");
            break;
        }
    }

    return EXIT_SUCCESS;
}