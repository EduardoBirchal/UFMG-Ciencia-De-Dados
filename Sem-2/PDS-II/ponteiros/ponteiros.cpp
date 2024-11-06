#include <iostream>
#include <string>

using namespace std;

// 0) Crie uma função que receba três variveis inteiras como parâmetro, da seguinte forma:
//    - A primeira deve ser passada por valor
//    - A segunda deve ser passada por referência 
//    - A terceira deve ser passada "por referência" usando apontadores (passagem "por referência" disponível em C)
//    A função deverá somar 1 ao valor de cada uma das 3 variáveis e retornar.

void f0 (int n1, int &n2, int *n3) {
    n1++;
    n2++;
    (*n3)++;
}


int main() {

    // 1) Declare uma variável do tipo inteiro e preencha com o valor informado na entrada
    int i1;
    cin >> i1;
    
    
    // 2) Declare um ponteiro para inteiros e inicialize com valor nulo
    int *p2 = nullptr;
    

    // 3) Declare um vetor de inteiros com tamanho informado na entrada e preencha com dados lidos da entrada
    int tam3;
    cin >> tam3;

    int* vet3 = new int[tam3];

    for (int i = 0; i < tam3; i++) {
        cin >> vet3[i];
    }
    

    // 4) Imprima o ENDEREÇO da variável declarada em (1)
    cout << &i1 << endl;


    // 5) Imprima o VALOR da variável declarada em (1)
    cout << i1 << endl;


    // 6) Imprima o ENDEREÇO da variável declarada em (2)
    cout << &p2 << endl;
    
    
    // 7) Imprima o VALOR da variável declarada em (2)
    cout << p2 << endl;
    

    // 8) Imprima o ENDEREÇO da variável declarada em (3)
    cout << &vet3 << endl;
    

    // 9) Imprima o ENDEREÇO da primeira posição da variável declarada em (3)
    cout << vet3 << endl;
    

    // 10) Imprima o VALOR da primeira posição da variável declarada em (3)
    cout << vet3[0] << endl;
    

    // 11) Atribua o ENDEREÇO da variável declarada em (1) à variável declarada em (2)
    p2 = &i1;
    

    // 12) Imprima o VALOR da variável declarada em (2)
    cout << p2 << endl;
    

    // 13) Imprima o VALOR guardado no ENDEREÇO apontado por (2)
    cout << *p2 << endl;


    // 14) Coloque o VALOR '5' no ENDEREÇO apontado por (2)
    *p2 = 5;
    

    // 15) Imprima o VALOR da variável declarada em (1)
    cout << i1 << endl;
    

    // 16) Atribua o VALOR da variável (3) à variável declarada em (2)
    p2 = vet3;
    

    // 17) Imprima o VALOR da variável declarada em (2)
    cout << p2 << endl;
    

    // 18) Imprima o VALOR guardado no ENDEREÇO apontado por (2)
    cout << *p2 << endl;
    

    // 19) Atribua o ENDEREÇO da primeira posição de (3) à variável declarada em (2)
    p2 = vet3;
    

    // 20) Compare o valor variáveis (2) e (3), imprimindo 'S' se forem iguais e 'N' se forem diferentes
    if (p2 == vet3)
        cout << "S" << endl;
    else
        cout << "N" << endl;


    // 21) Imprima o VALOR da variável declarada em (2)
    cout << p2 << endl;
    

    // 22) Imprima o VALOR guardado no ENDEREÇO apontado por (2)
    cout << *p2 << endl;
    

    // 23) Multiplique todos os valores do vetor declarado em (3) por '10', porém manipulando apenas a variável (2)
    for (int i = 0; i < tam3; i++) {
        *(p2 + i) *= 10;
    }
    

    // 24) Imprima os elementos de (3) a partir variável do vetor utilizando a notação [] (colchetes)
    cout << vet3[0];

    for (int i = 1; i < tam3; i++) {
        cout << " " << vet3[i];
    }

    cout << endl;
    

    // 25) Imprima os elementos de (3) a partir variável do vetor utilizando a notação ponteiro/deslocamento
    // Ou seja, você NÃO deve efetivamente alterar o valor do ponteiro inicial de (3)
    cout << *vet3;

    for (int i = 1; i < tam3; i++) {
        cout << " " << *(vet3+i);
    }

    cout << endl;
    

    // 26) Imprima os elementos de (3) utilizando a variável (2) e a notação ponteiro/deslocamento
    // Ou seja, você NÃO deve efetivamente alterar o valor do ponteiro inicial de (2)
    cout << *p2;

    for (int i = 1; i < tam3; i++) {
        cout << " " << *(p2+i);
    }

    cout << endl;
    

    // 27) Atribua o ENDEREÇO da última posição de (3) à variável declarada em (2)
    p2 = &(vet3[tam3 - 1]);
    

    // 28) Imprima o VALOR da variável declarada em (2)
    cout << p2 << endl;
    

    // 29) Imprima o VALOR guardado no ENDEREÇO apontado por (2)
    cout << *p2 << endl;
    

    // 30) Declare um ponteiro para ponteiro e o inicialize com o ENDEREÇO da variável (2)
    int** p30 = &p2;
    

    // 31) Imprima o VALOR da variável declarada em (30)
    cout << p30 << endl;
    

    // 32) Imprima o ENDEREÇO da variável declarada em (30)
    cout << &p30 << endl;
    

    // 33) Imprima o VALOR guardado no ENDEREÇO apontado por (30)
    cout << *p30 << endl;
    

    // 34) Imprima o VALOR guardado no ENDEREÇO do ponteiro apontado por (30)
    cout << **p30 << endl;
    

    // 35) Crie 3 variáveis interiras e leia o valor delas da entrada
    int i35_1, i35_2, i35_3;

    cin >> i35_1;
    cin >> i35_2;
    cin >> i35_3;


    // 36) Chame a função criada em (0) passando as 3 variáveis criadas em (35) como parâmetro.
    f0(i35_1, i35_2, &i35_3);

    
    // 37) Imprima o valor das 3 variáveis criadas em (35)
    cout << i35_1 << " " << i35_2 << " " << i35_3 << endl;
    
    delete vet3;
    return 0;
}