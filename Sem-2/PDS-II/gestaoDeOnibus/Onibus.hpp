#include <string>
#include <iostream>

using namespace std;

#ifndef ONIBUS_H
#define ONIBUS_H

#define INVALIDO -1

struct Onibus {
    string _placa;
    int _lotacaoMax, _lotacao;

    Onibus (string placa, int lotacaoMax);
    
    int subir_passageiros (int num);
    int descer_passageiros (int num);
    int transferir_passageiros (Onibus &onibusAlvo, int num);
    void imprimir_estado ();
};

#endif