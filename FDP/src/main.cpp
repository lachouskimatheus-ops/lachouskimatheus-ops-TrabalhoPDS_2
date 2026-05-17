#include <iostream>
#include "../include/MesaFDP.hpp"
#include "../include/JogadorFDP.hpp"
#include "../include/BaralhoSujo.hpp"
#include "../include/Placar.hpp"

int main() {
    BaralhoSujo* baralho = new BaralhoSujo();
    Placar* placar = new Placar();

    MesaFDP mesa(baralho, placar);

    JogadorFDP* j1 = new JogadorFDP(1, "Thiago", 5);
    JogadorFDP* j2 = new JogadorFDP(2, "Matheus", 5);
    JogadorFDP* j3 = new JogadorFDP(3, "Joao", 5);
    JogadorFDP* j4 = new JogadorFDP(4, "Maria", 5);

    mesa.adicionarJogador(j1);
    mesa.adicionarJogador(j2);
    mesa.adicionarJogador(j3);
    mesa.adicionarJogador(j4);

    mesa.iniciarPartida();

    delete j1;
    delete j2;
    delete j3;
    delete j4;
    delete baralho;
    delete placar;

    std::cout << "\nJogo encerrado.\n";
    return 0;
}