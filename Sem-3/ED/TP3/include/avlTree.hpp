#ifndef AVLTREE_HPP
#define AVLTREE_HPP

#include <string>
#include <iostream>
#include "utils.hpp"

// Helper para comparação, que sabe lidar com ponteiros
template<typename T>
struct Comparator {
    static bool isLess(const T& a, const T& b) { return a < b; }
    static bool isEqual(const T& a, const T& b) { return a == b; }
};
template<typename T>
struct Comparator<T*> {
    static bool isLess(T* a, T* b) { return *a < *b; }
    static bool isEqual(T* a, T* b) { return *a == *b; }
};


template<class T>
class avlTree {
private:
    struct Node {
        T data;
        Node* left;
        Node* right;
        int height;
        Node(const T& val) : data(val), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;
    bool deallocateNodes; // Flag para desalocar dados se T for ponteiro

    int height(Node* N) { return (N == nullptr) ? 0 : N->height; }
    int max(int a, int b) { return (a > b) ? a : b; }
    int getBalance(Node* N) { return (N == nullptr) ? 0 : height(N->left) - height(N->right); }

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    Node* insert(Node* node, const T& data) {
        if (node == nullptr) return(new Node(data));

        if (Comparator<T>::isLess(data, node->data))
            node->left = insert(node->left, data);
        else if (Comparator<T>::isLess(node->data, data))
            node->right = insert(node->right, data);
        else return node; // Duplicatas não são inseridas

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);

        if (balance > 1 && Comparator<T>::isLess(data, node->left->data)) return rightRotate(node);
        if (balance < -1 && Comparator<T>::isLess(node->right->data, data)) return leftRotate(node);
        if (balance > 1 && Comparator<T>::isLess(node->left->data, data)) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && Comparator<T>::isLess(data, node->right->data)) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }
    
    Node* findNode(Node* node, const T& data) const {
        if (node == nullptr) return nullptr;

        if (Comparator<T>::isLess(data, node->data))
            return findNode(node->left, data);
        else if (Comparator<T>::isLess(node->data, data))
            return findNode(node->right, data);
        else
            return node; // Encontrado
    }

    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            if (deallocateNodes) {
                NodeDeleter<T>::destroy(node->data);
            }
            delete node;
        }
    }

public:
    explicit avlTree(bool doDeallocateNodes = false) : root(nullptr), deallocateNodes(doDeallocateNodes) {}
    ~avlTree() { destroyTree(root); }

    void insert(const T& data) { root = insert(root, data); }

    T find(const T& data) const {
        Node* result = findNode(root, data);
        if (result) {
            return result->data;
        }
        return T{}; // Retorna nullptr para tipos de ponteiro, 0 para int, etc.
    }
};

#endif // AVLTREE_HPP
