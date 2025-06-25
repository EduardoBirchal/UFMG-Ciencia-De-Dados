#include <iostream>
#include "escalonador.hpp"
#include "rede.hpp"
#include "pacote.hpp"
#include "evento.hpp"
#include "armazem.hpp"

int main() {
    // 1. Parâmetros da Simulação
    int max_transporte = 2;
    int latencia_transporte = 20;
    int intervalo_transportes = 110; 
    int custo_remocao = 1;
    int num_armazens = 4;
    
    int num_pacotes = 3;
    int max_eventos = 2 * num_pacotes; 

    // 2. Inicialização do Sistema
    Rede rede(num_armazens, max_transporte, custo_remocao);
    Escalonador escalonador(max_eventos, num_pacotes, latencia_transporte, intervalo_transportes, &rede);

    // 3. Configuração da Rede
    std::cout << "--- Configurando a Rede de Armazens ---\n";
    for (int i = 0; i < num_armazens; ++i) {
        rede.addArmazem(i);
    }
    rede.addConexao(0, 1);
    rede.addConexao(1, 2);
    rede.addConexao(1, 3);
    std::cout << "Rede criada com " << num_armazens << " armazens.\n\n";

    // 4. Agendamento de Postagens e Transportes
    std::cout << "--- Agendando Eventos Iniciais ---\n";
    
    // Pacotes
    Pacote* p1 = new Pacote(10, 1101, 0, 2, 0); // De 0 para 2
    Pacote* p2 = new Pacote(15, 1102, 3, 0, 1); // De 1 para 3
    Pacote* p3 = new Pacote(20, 1103, 2, 3, 2); // De 3 para 0
    
    // Agendamento de postagens
    escalonador.agendar<EventoPostagem>(p1, p1->getOrigem(), p1->getDestinoFinal(), 0, p1->getHoraPostagem());
    escalonador.agendar<EventoPostagem>(p2, p2->getOrigem(), p2->getDestinoFinal(), 0, p2->getHoraPostagem());
    escalonador.agendar<EventoPostagem>(p3, p3->getOrigem(), p3->getDestinoFinal(), 0, p3->getHoraPostagem());
    
    std::cout << num_pacotes << " pacotes postados.\n\n";

    // 5. Loop Principal da Simulação
    std::cout << "--- Iniciando a Simulacao ---\n\n";
    while (!escalonador.vazio()) {
        escalonador.simularProximo();
    }

    std::cout << "\n--- Simulacao Concluida ---\n";

    return 0;
}
