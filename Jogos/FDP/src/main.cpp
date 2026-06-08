#include <iostream>

#include "../include/BaralhoSujo.hpp"
#include "../include/Placar.hpp"
#include "../include/MesaFDP.hpp"
#include "../include/JogadorFDP.hpp"
#include "../include/Servidor.hpp"

int main() {
    std::cout << "Inicializando o servidor FDP (Fodinha)..." << std::endl;

    // 1. Instancia as dependências vitais do jogo
    BaralhoSujo baralho;
    Placar placar;
    
    // 2. Cria a mesa passando o baralho e o placar
    MesaFDP mesa(&baralho, &placar);
    
    mesa.adicionarJogador(new JogadorFDP(0, "Jogador 1"));
    mesa.adicionarJogador(new JogadorFDP(1, "Jogador 2"));
    mesa.adicionarJogador(new JogadorFDP(2, "Jogador 3"));
    mesa.adicionarJogador(new JogadorFDP(3, "Jogador 4"));
    
    mesa.iniciarPartida();

    Servidor servidor(&mesa);
    
    servidor.iniciar(8080); 

    return 0;
}