#include <iostream>
#include "../Core/CoreBaralho1/baralho.h"
#include "poker.h"

int main() {
    Baralho baralho;
    baralho.embaralhar();

    Poker jogo;

    for (int i = 0; i < 5; i++) {
        jogo.receberCarta(baralho.retirarCarta());
    }

    std::cout << "Mao:\n";

    const std::vector<Carta>& mao = jogo.verMao();

    for (int i = 0; i < mao.size(); i++) {
        std::cout << "- " << mao[i].cartaString() << "\n";
    }

    std::cout << "\nJogada: " << jogo.nomeJogada() << "\n";

    return 0;
}