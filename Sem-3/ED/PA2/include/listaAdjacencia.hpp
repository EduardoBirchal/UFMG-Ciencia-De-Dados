#ifndef LIST_HPP
#define LIST_HPP

template <class tipo> class nodo {
    public:
        nodo (tipo dataIn = NULL, nodo *proxIn = nullptr) {
            this->data = dataIn;
            this->prox = proxIn;
        }
        
        ~nodo();

        void setProx (nodo *proxIn) {
            this->prox = proxIn;
        }

    private:
        tipo data;
        nodo *prox;
};

class listaAdjacencia{
    public:
        listaAdjacencia();
        ~listaAdjacencia();

    private:
        nodo <int> *head;
};

#endif