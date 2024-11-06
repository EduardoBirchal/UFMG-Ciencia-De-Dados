#include "Onibus.hpp"

Onibus::Onibus (string placa, int lotacaoMax) {
    _placa = placa;
    _lotacaoMax = lotacaoMax;
    _lotacao = 0;
}

int Onibus::subir_passageiros (int num) {
    if (_lotacao + num <= _lotacaoMax && num > 0) {
        _lotacao += num;
        return _lotacao;
    }
    else 
        return INVALIDO;
}

int Onibus::descer_passageiros (int num) {
    if (num <= _lotacao && num > 0) {
        _lotacao -= num;
        return _lotacao;
    }
    else
        return INVALIDO;
}

int Onibus::transferir_passageiros (Onibus &onibusAlvo, int num) {
    if(this->descer_passageiros (num) != INVALIDO && onibusAlvo.subir_passageiros (num) != INVALIDO) {
        return this->_lotacao;
    }
    else
        return INVALIDO;
}

void Onibus::imprimir_estado () {
    cout << _placa << " (" << _lotacao << "/" << _lotacaoMax << ")" << endl;
}