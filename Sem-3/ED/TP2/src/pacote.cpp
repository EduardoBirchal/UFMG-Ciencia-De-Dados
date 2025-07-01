#include "pacote.hpp"

// Construtor com a lista de inicialização na ordem correta
Pacote::Pacote(int hora_postagem, int id_entrada, int id_origem, int id_destino_final, int id_sequencial) : // CORREÇÃO: A ordem agora corresponde à declaração em pacote.hpp
                                                                                                            idSequencial(id_sequencial),
                                                                                                            idEntrada(id_entrada),
                                                                                                            horaPostagem(hora_postagem),
                                                                                                            idOrigem(id_origem),
                                                                                                            idDestinoFinal(id_destino_final),
                                                                                                            idDestinoAtual(id_origem),
                                                                                                            idArmazemAtual(id_origem),
                                                                                                            idSecaoAtual(0),
                                                                                                            estado(EstadoPacote::inicial),
                                                                                                            rotaArmazens(false)
{ // Inicializa a fila sem desalocação de ponteiros
    // O corpo do construtor pode ficar vazio
}

// Destrutor
Pacote::~Pacote()
{
    // Nada a fazer
}

// Retorna o ID sequencial
int Pacote::getID() const
{
    return this->idSequencial;
}

// Retorna o ID recebido na entrada
int Pacote::getIDEntrada() const
{
    return this->idEntrada;
}

// Retorna a hora da postagem
int Pacote::getHoraPostagem() const
{
    return this->horaPostagem;
}

// Retorna o ID de origem
int Pacote::getOrigem() const
{
    return this->idOrigem;
}

// Retorna o ID de destino final
int Pacote::getDestinoFinal() const
{
    return this->idDestinoFinal;
}

// Retorna o ID do próximo destino na rota
int Pacote::getDestinoAtual() const
{
    return this->idDestinoAtual;
}

// Retorna o ID da localização atual
int Pacote::getArmazemAtual() const
{
    return this->idArmazemAtual;
}

// Retorna o ID da seção atual
int Pacote::getSecaoAtual() const
{
    return this->idSecaoAtual;
}

// Retorna o estado atual
EstadoPacote Pacote::getEstado() const
{
    return this->estado;
}

// Retorna uma referência para a fila de rotas para manipulação externa
Queue<int> &Pacote::getRota()
{
    return this->rotaArmazens;
}

// Define o novo estado
void Pacote::setEstado(EstadoPacote novo_estado)
{
    this->estado = novo_estado;
}

// Define a localização atual
void Pacote::setArmazemAtual(int id_atual)
{
    this->idArmazemAtual = id_atual;
}

// Define a seção atual
void Pacote::setSecaoAtual(int id_secao_atual)
{
    this->idSecaoAtual = id_secao_atual;
}

// Define o próximo destino na rota
void Pacote::setDestinoAtual(int id_destino)
{
    this->idDestinoAtual = id_destino;
}

// --- Novos Métodos de Lógica ---

// Atualiza o destino atual com base na fila de rotas
void Pacote::atualizarDestinoAtual()
{
    if (!this->rotaArmazens.isEmpty())
    {
        // Se a rota não estiver vazia, pega o próximo armazém como destino
        this->setDestinoAtual(this->rotaArmazens.dequeue());
    }
    else if (this->getDestinoAtual() == this->getDestinoFinal())
    {
        // Se a rota estiver vazia e já estiver no destino final, marca como entregue
        this->pacoteEntregue();
    }
}

// Método chamado quando o pacote é entregue
void Pacote::pacoteEntregue()
{
    // Por enquanto, este método não faz nada
    // No futuro, poderia mudar o estado do pacote para 'entregue'
    // this->setEstado(EstadoPacote::entregue);
}
