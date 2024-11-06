#include <string>
#include <vector>
#include "Onibus.hpp"

using namespace std;

#ifndef EMPRESA_H
#define EMPRESA_H

#define MAX_ONIBUS 20

struct Empresa {
    int _numOnibus;
    Onibus* _vetorOnibus[MAX_ONIBUS];

    Empresa ();
    
    Onibus* adicionar_onibus (string placa, int lotacao);
    Onibus* busca_onibus (string placa);
    void imprimir_estado ();
};

#endif