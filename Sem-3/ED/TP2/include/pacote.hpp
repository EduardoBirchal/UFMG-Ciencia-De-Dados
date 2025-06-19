#ifndef PACOTE_H
#define PACOTE_H

enum estadosPacote {  };

class Packet 
{
private:
    int tempoArmazenamento, tempoTransporte, estado;
public:
    int getTempoArmazenamento ()            { return this->tempoArmazenamento; }
    void setTempoArmazenamento (int n)      { this->tempoArmazenamento = n; }

    int getTempoTransporte ()          { return this->tempoTransporte; }
    void setTempoTransporte (int n)    { this->tempoTransporte = n; }
};

#endif