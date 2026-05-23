#include <iostream>
#include <vector>
#include "Menu.hpp"
#include "Mesa.hpp"
#include "BaralhoSujo.hpp"
#include "JuizPaulista.hpp"
#include "Jogador_Truco.hpp"

void iniciar_truco() {

    JuizPaulista* juiz = new JuizPaulista();
    BaralhoSujo* baralho = new BaralhoSujo();
    
    Mesa mesa(juiz, baralho);

    Jogador_Truco* j1 = new Jogador_Truco("Voce");
    Jogador_Truco* j2 = new Jogador_Truco("Bot 1 (Oponente)");
    Jogador_Truco* j3 = new Jogador_Truco("Parceiro");
    Jogador_Truco* j4 = new Jogador_Truco("Bot 2 (Oponente)");

    mesa.adicionarJogador(j1);
    mesa.adicionarJogador(j2);
    mesa.adicionarJogador(j3);
    mesa.adicionarJogador(j4);

    std::cout << "--- Iniciando Partida de Truco ---" << std::endl;

    mesa.prepararRodada();
    mesa.jogarTurno();

    std::cout << "\nFim da rodada!" << std::endl;
    std::cout << "Pressione Enter para voltar ao menu principal...";
    std::cin.ignore();
    std::cin.get();

    delete j1; delete j2; delete j3; delete j4;
    delete juiz;
    delete baralho;
}

int main() {
    Menu menu;

    menu.addOpcao("Jogar Truco Paulista (Sujo)", iniciar_truco);
    
    while (true) {
        menu.exibir();
    }

    return 0;
}