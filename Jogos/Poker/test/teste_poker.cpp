#include <iostream>
#include "Baralho.hpp"
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
Poker jogador1;
jogador1.receberCarta(Carta(Valor::As, Naipe::Paus));
jogador1.receberCarta(Carta(Valor::As, Naipe::Copa));
jogador1.receberCarta(Carta(Valor::Cinco, Naipe::Ouro));
jogador1.receberCarta(Carta(Valor::Sete, Naipe::Espada));
jogador1.receberCarta(Carta(Valor::Nove, Naipe::Paus));

Poker jogador2;
jogador2.receberCarta(Carta(Valor::Rei, Naipe::Paus));
jogador2.receberCarta(Carta(Valor::Rei, Naipe::Copa));
jogador2.receberCarta(Carta(Valor::Cinco, Naipe::Espada));
jogador2.receberCarta(Carta(Valor::Sete, Naipe::Ouro));
jogador2.receberCarta(Carta(Valor::Nove, Naipe::Copa));

std::cout << "\nJogador 1: " << jogador1.nomeJogada() << std::endl;
std::cout << "Jogador 2: " << jogador2.nomeJogada() << std::endl;

int vencedor = jogador1.compararCom(jogador2);

if (vencedor == 1) {
    std::cout << "Vencedor: Jogador 1" << std::endl;
} else if (vencedor == -1) {
    std::cout << "Vencedor: Jogador 2" << std::endl;
} else {
    std::cout << "Empate" << std::endl;
}