#include <iostream>
#include <exception>
#include <vector>

using namespace std;

class NegativeResultExpression : exception
{
private:
    int valor;
public:
    NegativeResultExpression(int n) : exception() 
    {
        this->valor = n;
    }

    virtual const char* what() const throw()
    {
        return "Erro: Resultado Negativo";
    }

    int getValor() { return valor; }
};

int main() {
    vector<int> vec = {-20, -10, 0, 10, 20};
    int input;

    try
    {
        cin >> input;

        if (input > int(vec.size()) - 1)
            throw std::out_of_range("Erro: Parametro Invalido");

        int valor = vec.at(input);

        if (valor < 0)
            throw NegativeResultExpression(valor);

        if (valor == 0)
            throw std::logic_error("Erro: O resultado nao deve ser zero");

        cout << valor << endl;
    }
    catch(NegativeResultExpression e)
    {
        cout << e.what() << endl;
        cout << e.getValor() << endl;
    }
    catch(const exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}