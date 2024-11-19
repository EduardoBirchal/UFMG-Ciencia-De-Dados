#include "list.hpp"

#include <iostream>

void List::insertNode(int data) {
    Node* aux = new Node;
    aux->data = data;
    aux->next = nullptr;
    if (head == nullptr) {
        head = aux;
        tail = aux;
    } else {
        tail->next = aux;
        tail = aux;
    }
}

void List::display() {
    Node* aux = head;
    while (aux != nullptr) {
        cout << aux->data << " ";
        aux = aux->next;
    }
    cout << endl;
}

// Você deverá implementar apenas esse método
void List::inverte() {
    tail = head;

    Node* anterior = head;
    Node* atual = anterior->next;
    Node* prox = atual->next;

    anterior->next = nullptr;

    while (prox != nullptr) {
        atual->next = anterior;

        anterior = atual;
        atual = prox;
        prox = atual->next;
    }

    atual->next = anterior;
    head = atual;
}