#include <iostream>
#include <string>

#include "Empresa.hpp"

using namespace std;

int main() {
    string input;

    Empresa* empresaAtual = new Empresa();

    while (true) {
        cin >> input;
        
        if (input == "C") {
            string placa;
            int lotacao;

            cin >> placa;
            cin >> lotacao;

            Onibus *novoOnibus = empresaAtual->adicionar_onibus(placa, lotacao);

            if (novoOnibus == nullptr) {
                cout << "ERRO : onibus repetido" << endl;
            }
            else {
                cout << "novo onibus adicionado" << endl;
            }
        }

        else if (input == "S") {
            string placa;
            int num, resultado;
            Onibus* alvo;

            cin >> placa;
            cin >> num;

            alvo = empresaAtual->busca_onibus(placa);

            if (alvo == nullptr) {
                cout << "ERRO : onibus inexistente" << endl;
            }
            else {
                resultado = alvo->subir_passageiros(num);

                if (resultado != INVALIDO) {
                    cout << "passageiros subiram com sucesso" << endl;
                }
                else {
                    cout << "ERRO: onibus lotado" << endl;
                }
            }
        }

        else if (input == "D") {
            string placa;
            int num, resultado;
            Onibus* alvo;

            cin >> placa;
            cin >> num;

            alvo = empresaAtual->busca_onibus(placa);

            if (alvo == nullptr) {
                cout << "ERRO : onibus inexistente" << endl;
            }
            else {
                resultado = alvo->descer_passageiros(num);

                if (resultado != INVALIDO) {
                    cout << "passageiros subiram com sucesso" << endl;
                }
                else {
                    cout << "ERRO: faltam passageiros" << endl;
                }
            }
        }

        else if (input == "T") {
            string placa1, placa2;
            int num, resultado;
            Onibus *fonte, *alvo;

            cin >> placa1;
            cin >> placa2;
            cin >> num;

            fonte = empresaAtual->busca_onibus(placa1);
            alvo = empresaAtual->busca_onibus(placa2);

            if (fonte != nullptr && alvo != nullptr) {
                resultado = fonte->transferir_passageiros(*alvo, num);

                if (resultado != INVALIDO) {
                    cout << "transferencia de passageiros efetuada" << endl;
                }
                else {
                    cout << "ERRO : transferencia cancelada" << endl;
                }
            }
            else {
                cout << "ERRO : onibus inexistente" << endl;
            }
        }

        else if (input == "I") {
            empresaAtual->imprimir_estado();
        }

        else if (input == "F") {
            break;
        }

        else {
            cout << "ERRO: Input invalido" << endl;
        }
    }

    return 0;
}