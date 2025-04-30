#ifndef LIST_HPP
#define LIST_HPP

#include <vector>

template <class tipo> class nodo {
    public:
        nodo (tipo dataIn = NULL, vector<nodo*> vizinhosIn = new vector<nodo*>) {
            this->data = dataIn;
            this->vizinhos = vizinhosIn;
        }
        
        ~nodo();

        void addVizinho (nodo *proxVizinho) {
            this->vizinhos = proxIn;
        }

    private:
        tipo data;
        vector<nodo*> vizinhos;
};

class listaAdjacencia{
    public:
        listaAdjacencia();
        ~listaAdjacencia();

    private:
        nodo <int> *head;
};

#endif