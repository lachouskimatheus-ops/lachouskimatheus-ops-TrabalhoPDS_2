#include <iostream>
#include <cassert>

#include "Baralho.hpp"
#include "poker.h"

void testarMaoAleatoria() {
    std::cout << "Testando mao aleatoria... ";

    Baralho baralho;
    baralho.embaralhar();

    Poker jogo;

    for (int i = 0; i < 5; i++) {
        jogo.receberCarta(baralho.retirarCarta());
    }

    assert(jogo.tamanhoMao() == 5);

    std::cout << "OK!" << std::endl;

    std::cout << "\nMao gerada:\n";
    const std::vector<Carta>& mao = jogo.verMao();

    for (int i = 0; i < (int)mao.size(); i++) {
        std::cout << "- " << mao[i].cartaString() << "\n";
    }

    std::cout << "Jogada: " << jogo.nomeJogada() << "\n\n";
}

void testarLimiteDeCartas() {
    std::cout << "Testando limite de 5 cartas... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::As, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Rei, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Dama, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Valete, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Dez, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Nove, Naipe::Paus));

    assert(jogo.tamanhoMao() == 5);

    std::cout << "OK!" << std::endl;
}

void testarRoyalFlush() {
    std::cout << "Testando Royal Flush... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::Dez, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Valete, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Dama, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Rei, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::As, Naipe::Paus));

    assert(jogo.avaliarMao() == 9);
    assert(jogo.nomeJogada() == "Royal Flush");

    std::cout << "OK!" << std::endl;
}

void testarFullHouse() {
    std::cout << "Testando Full House... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::As, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::As, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::As, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Rei, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Rei, Naipe::Espada));

    assert(jogo.avaliarMao() == 6);
    assert(jogo.nomeJogada() == "Full House");

    std::cout << "OK!" << std::endl;
}

void testarComparacaoDeMaos() {
    std::cout << "Testando comparacao de maos... ";

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

    assert(jogador1.nomeJogada() == "Um Par");
    assert(jogador2.nomeJogada() == "Um Par");
    assert(jogador1.compararCom(jogador2) == 1);

    std::cout << "OK!" << std::endl;
}

int main() {
    std::cout << "=== INICIANDO TESTES DO POKER ===\n\n";

    testarMaoAleatoria();
    testarLimiteDeCartas();
    testarRoyalFlush();
    testarFullHouse();
    testarComparacaoDeMaos();

    std::cout << "\n=====================================\n";
    std::cout << " TODOS OS TESTES DO POKER PASSARAM!\n";
    std::cout << "=====================================\n";

    return 0;
}