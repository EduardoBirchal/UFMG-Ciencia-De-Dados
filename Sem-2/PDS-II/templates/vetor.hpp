#ifndef VETOR_H
#define VETOR_H

#include <iostream>

template <class tipo>

class Vetor 
{
    private:
        tipo *_array;
        int _tamanho, _atual;

    public:
        Vetor(int tamanho) {
            this-> _tamanho = tamanho;
            this->_atual = 0;
            this->_array = new tipo[tamanho];
        }

        Vetor(const Vetor& original) {
            this-> _tamanho = original._tamanho;
            this->_atual = original._atual;
            this->_array = new tipo[_tamanho];

            for (int i = 0; i < _atual; i++) {
                _array[i] = original._array[i];
            }
        }

        int GetTamanho() { return this->_tamanho; }

        int GetAtual() { return this->_atual; }

        tipo GetElemento(int indice) {
            return this->_array[indice];
        }

        void SetElemento(int indice, tipo elemento) {
            _array[indice] = elemento;
            _atual++;
        }

        void AdicionaElemento(tipo elemento) {
            SetElemento(_atual, elemento);
        }

        void Imprime() {
            for (int i = 0; i < _atual; i++) {
                std::cout << _array[i] << " ";
            }
            
            std::cout << std::endl;
        }
};

#endif