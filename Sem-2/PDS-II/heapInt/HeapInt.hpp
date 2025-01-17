#include <iostream>

#ifndef HEAPINT_H
#define HEAPINT_H

class HeapInt {
private:
    int *valor;

public:
    HeapInt (int n = 0);

    HeapInt (const HeapInt& original);

    ~HeapInt();

    int getValor();

    void setValor(int n);

    HeapInt operator = (int segundo);

    HeapInt operator = (const HeapInt& segundo);

    HeapInt operator + (const HeapInt& segundo);

    HeapInt operator + (int segundo);

    HeapInt operator - (const HeapInt& segundo);

    HeapInt operator - (int segundo);

    bool operator == (const HeapInt& segundo);

    friend std::ostream& operator << (std::ostream& cout, HeapInt& segundo);

    friend std::istream& operator >> (std::istream& cin, HeapInt& segundo);
};

#endif