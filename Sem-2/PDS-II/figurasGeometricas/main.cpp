#include <iostream>
#include <string>
#include <cmath>
#include <vector>

class Ponto {
    private:
        int x, y;
    
    public:
        Ponto (int x = 0, int y = 0): x(x), y(y) {}

        int GetX () {
            return this->x;
        }

        int GetY () {
            return this->x;
        }

        int SetX (int n) {
            this->x = n;
            return n;
        }
        
        int SetY (int n) {
            this->y = n;
            return n;
        }

        void SetCoords (int nx, int ny) {
            SetX(nx);
            SetY(ny);
        }
};

class FiguraGeometrica {
    protected:
        Ponto centro;

    public:
        FiguraGeometrica (int xCentro = 0, int yCentro = 0)
            : centro {xCentro, yCentro} {};

        virtual void Desenha () {
            std::cout << centro.GetX() << " " << centro.GetY() << " ";
        }

        virtual double CalculaArea () = 0;
};

class Retangulo : public FiguraGeometrica {
    protected:
        int ladoX, ladoY;

    public:
        Retangulo (int xCentro = 0, int yCentro = 0, int ladoX = 1, int ladoY = 1) 
            : FiguraGeometrica(xCentro, yCentro), 
              ladoX(ladoX), 
              ladoY(ladoY) {}

        void Desenha() override {
            FiguraGeometrica::Desenha();
            std::cout << "RETANGULO" << std::endl;
        }

        double CalculaArea() override {
            return ladoX * ladoY;
        }
};

class Circulo : public FiguraGeometrica {
    protected:
        int raio;

    public:
        Circulo (int xCentro, int yCentro, int raio) 
            : FiguraGeometrica(xCentro, yCentro), 
              raio(raio) {}

        void Desenha() override {
            FiguraGeometrica::Desenha();
            std::cout << "CIRCULO" << std::endl;
        }

        double CalculaArea() override {
            return raio * raio * M_PI;
        }
};

class Triangulo : public FiguraGeometrica {
    protected:
        int base, altura;

    public:
        Triangulo (int xCentro = 0, int yCentro = 0, int base = 1, int altura = 1) 
            : FiguraGeometrica(xCentro, yCentro), 
              base(base), 
              altura(altura) {}

        void Desenha() override {
            FiguraGeometrica::Desenha();
            std::cout << "TRIANGULO" << std::endl;
        }

        double CalculaArea() override {
            return base * altura * 0.5;
        }
};

using namespace std;

int main() {
    
    vector<FiguraGeometrica*> figuras = {};

    while (true) {
        string input;

        std::cin.clear();
        cin >> input;

        if (input == "R") {
            int xCentro, yCentro, ladoX, ladoY;

            cin >> xCentro;
            cin >> yCentro;
            cin >> ladoX; 
            cin >> ladoY;

            figuras.push_back(new Retangulo(xCentro, yCentro, ladoX, ladoY));
        }

        if (input == "C") {
            int xCentro, yCentro, raio;

            cin >> xCentro;
            cin >> yCentro;
            cin >> raio; 

            figuras.push_back(new Circulo(xCentro, yCentro, raio));
        }

        if (input == "T") {
            int xCentro, yCentro, base, altura;

            cin >> xCentro;
            cin >> yCentro;
            cin >> base; 
            cin >> altura;

            figuras.push_back(new Triangulo(xCentro, yCentro, base, altura));
        }

        if (input == "D") {
            for (FiguraGeometrica* figura : figuras) {
                figura->Desenha();
            }
        }

        if (input == "A") {
            double areaTotal = 0;

            for (FiguraGeometrica* figura : figuras) {
                areaTotal += figura->CalculaArea();
            }

            cout << areaTotal << endl;  
        }

        if (input == "E") {
            break;
        }
    }

    return 0;
}