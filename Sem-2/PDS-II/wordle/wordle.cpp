#include <iostream>
#include <string>
#include <fstream>

#define NUM_TENTATIVAS 5
#define TAMANHO_PALAVRA 5

using namespace std;

// Testa se uma letra está correta ou não. Retorna o grau de corretude dela (0 pra incorreta, 1 pra correta mas posição incorreta, 2 para correta)
int analizaLetra (char c, size_t posInput, string str) {
    int valor = 2;
    size_t posOriginal;

    posOriginal = str.find(c);

    if (posOriginal == string::npos) {
        valor = 0;
    }
    else if (posOriginal != posInput) {
        valor = 1;
    }

    return valor;
}

// Loop principal de gameplay
bool tentativa (string chave, string *output, string *erradas) {
    bool venceu = true;
    int corretude;
    string input;

    cin >> input;

    for (int i = 0; i < TAMANHO_PALAVRA; i++) {
        corretude = analizaLetra(input[i], i, chave);

        switch (corretude)
        {
            case 2:
                *output += input[i];
                break;

            case 1:
                *output += tolower(static_cast<unsigned char>(input[i]));

                venceu = false;
                break;
            
            default:
                *output += '*';

                if (erradas->find(input[i]) == string::npos)
                    *erradas += input[i];

                venceu = false;
                break;
        }
    }

    return venceu;
}

int main () {
    // Declarando variáveis
    int maxChave, chaveEscolhida;
    string strBuffer, strChave, strErradas;
    bool ganhou;

    // Abrindo o arquivo
    ifstream palavras("palavras.txt", fstream::in);

    if (!palavras.is_open())
        return 1;
    
    // Pegando a chave numérica da palavra
    getline(palavras, strBuffer);
    maxChave = stoi(strBuffer);

    while (true) {
        cin >> chaveEscolhida;

        if (chaveEscolhida > 0 && chaveEscolhida <= maxChave) 
            break;
        
        cout << "Chave invalida. Tente novamente.\n";
    }

    // Pegando a palavra-chave
    for (int i = 1; i <= chaveEscolhida; i++) {
        getline(palavras, strChave);
    }

    // Loop de gameplay
    for (int i = 0; i < NUM_TENTATIVAS; i++) {
        string strFeedback;

        ganhou = tentativa(strChave, &strFeedback, &strErradas);
        cout << strFeedback << " (" << strErradas << ")\n";

        if (ganhou) {
            cout << "GANHOU!";
            break;
        }
    }

    if (!ganhou) {
        cout << "PERDEU! " << strChave;
    }

    return 0;
}