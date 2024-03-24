#include <stdio.h>
#include <stdlib.h>

#define JUROS_MES 1.01
#define DINHEIRO_INICIAL 500

float passaMes (float dinheiroInicial, float somaDinheiro) {
    return (dinheiroInicial + somaDinheiro) * JUROS_MES;
}

int main (int argc, char **argv) {
    float dinheiroAtual = DINHEIRO_INICIAL;

    dinheiroAtual = passaMes (dinheiroAtual, 0.0);      // Mês 1
    dinheiroAtual = passaMes (dinheiroAtual, 200.0);    // Mês 2
    dinheiroAtual = passaMes (dinheiroAtual, -50.0);    // Mês 3

    printf("%.2f\n", dinheiroAtual);

    return 0;
}