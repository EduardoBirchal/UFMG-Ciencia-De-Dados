#include <iostream>
#include <string>
#include <memory>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

using namespace std;

class teste
{
private:
    int n;

public:
    teste () {
        this->n = 0;
        cout << "Construtor " << this->n << endl;
    }

    teste (int num) {
        this->n = num;
        cout << "Construtor " << this->n << endl;
    }

    ~teste() {
        cout << "Destrutor " << this->n << endl;
    }

    void setValor (int num) {
        this->n = num;
    }
};

int main() {
    int opcao;

    cin >> opcao;

    if (opcao % 2 == 0) {
        for (int c = 1; c <= opcao; c++) {
            teste* obj = new teste(c);
            unique_ptr<teste> ptr(new teste(c));
        }
    }
    else {
        shared_ptr<teste> ptr(new teste());

        for (int c = 1; c <= opcao; c++) {
            shared_ptr<teste> new_ptr(ptr);
            new_ptr->setValor(c);
        }

        cout << ptr.use_count() << endl;
    }

    return 0;
}

#pragma GCC diagnostic pop