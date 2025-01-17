#include "HeapInt.hpp"


    HeapInt::HeapInt (int n) {
        this->valor = new int(n);
    }

    HeapInt::HeapInt (const HeapInt& original) {
        this->valor = new int(*(original.valor));
    }

    HeapInt::~HeapInt() {
        delete[] this->valor;
    }
    
    int HeapInt::getValor() {
        return *(this->valor);
    }
    
    void HeapInt::setValor (int n) {
        *(this->valor) = n;
    }

    HeapInt HeapInt::operator = (int segundo) {
        *(this->valor) = segundo;
        return HeapInt(segundo);
    }

    HeapInt HeapInt::operator = (const HeapInt& segundo) {
        *(this->valor) = *(segundo.valor);
        return HeapInt(segundo);
    }

    HeapInt HeapInt::operator + (int segundo) {
        return HeapInt(*(this->valor) + segundo);
    }

    HeapInt HeapInt::operator + (const HeapInt& segundo) {
        return HeapInt(*(this->valor) + *(segundo.valor));
    }

    HeapInt HeapInt::operator - (const HeapInt& segundo) {
        return HeapInt(*(this->valor) + ((*(segundo.valor)) * -1));
    }
    
    HeapInt HeapInt::operator - (int segundo) {
        return HeapInt(*(this->valor) - segundo);
    }

    bool HeapInt::operator == (const HeapInt& segundo) {
        return *(this->valor) == *(segundo.valor);
    }

    std::istream& operator >> (std::istream& cin, HeapInt& segundo) {
        int buffer;
        cin >> buffer;
        segundo.setValor(buffer);
        return cin;
    }

    std::ostream& operator << (std::ostream& cout, HeapInt& segundo) {
        cout << segundo.getValor();
        return cout;
    }