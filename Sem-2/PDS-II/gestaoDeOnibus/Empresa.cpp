#include "Empresa.hpp"

Empresa::Empresa () {
    _numOnibus = 0;

    for (int i = 0; i < MAX_ONIBUS; i++) {
        _vetorOnibus[i] = nullptr;
    }
}

Onibus* Empresa::busca_onibus (string placa) {
    for (int i = 0; i < _numOnibus; i++) {
        if (_vetorOnibus[i]->_placa == placa) {
            return _vetorOnibus[i];
        }
    }

    return nullptr;
}

Onibus* Empresa::adicionar_onibus(string placa, int lotacao) {
    if (this->busca_onibus(placa) == nullptr) {
        _vetorOnibus[_numOnibus] = new Onibus(placa, lotacao);
        _numOnibus++;

        return _vetorOnibus[_numOnibus-1];
    }

    return nullptr;
}

void Empresa::imprimir_estado () {
    for (int i = 0; i < _numOnibus; i++) {
        _vetorOnibus[i]->imprimir_estado();
    }
}