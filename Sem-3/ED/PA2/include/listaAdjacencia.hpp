#ifndef LIST_HPP
#define LIST_HPP

#include <vector>

class nodo {
    public:
        nodo (std::vector<int> dataIn = std::vector<int>(), nodo *proxIn = nullptr) {
            this->data = dataIn;
            this->prox = proxIn;
        }
        
        ~nodo() {
            delete this->prox;
        };

        void setProx (nodo *proxIn) {
            this->prox = proxIn;
        }

        void setData (std::vector<int> dataIn) {
            this->data = dataIn;
        }

    private:
        std::vector<int> data;
        nodo *prox;
};

class listaAdjacencia{
    public:
        listaAdjacencia(nodo *headIn = nullptr) {
            this->head = headIn;
        };

        ~listaAdjacencia() {
            delete this->tail;
        };

    private:
        nodo *head, *tail;
};

#endif