#include <iostream>
#include <cassert>
#include <vector>

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

    for (size_t i = 0; i < mao.size(); i++) {
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

void testarStraightFlush() {
    std::cout << "Testando Straight Flush... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::Cinco, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Seis, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Sete, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Oito, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Nove, Naipe::Copa));

    assert(jogo.avaliarMao() == 8);
    assert(jogo.nomeJogada() == "Straight Flush");

    std::cout << "OK!" << std::endl;
}

void testarQuadra() {
    std::cout << "Testando Quadra... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::Nove, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Nove, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Nove, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Nove, Naipe::Espada));
    jogo.receberCarta(Carta(Valor::Dois, Naipe::Paus));

    assert(jogo.avaliarMao() == 7);
    assert(jogo.nomeJogada() == "Quadra");

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

void testarFlush() {
    std::cout << "Testando Flush... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::As, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Tres, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Sete, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Nove, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Dama, Naipe::Ouro));

    assert(jogo.avaliarMao() == 5);
    assert(jogo.nomeJogada() == "Flush");

    std::cout << "OK!" << std::endl;
}

void testarSequencia() {
    std::cout << "Testando Sequencia... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::Quatro, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Cinco, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Seis, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Sete, Naipe::Espada));
    jogo.receberCarta(Carta(Valor::Oito, Naipe::Paus));

    assert(jogo.avaliarMao() == 4);
    assert(jogo.nomeJogada() == "Sequencia");

    std::cout << "OK!" << std::endl;
}

void testarSequenciaComAsBaixo() {
    std::cout << "Testando Sequencia com As baixo... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::As, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Dois, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Tres, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Quatro, Naipe::Espada));
    jogo.receberCarta(Carta(Valor::Cinco, Naipe::Paus));

    assert(jogo.avaliarMao() == 4);
    assert(jogo.nomeJogada() == "Sequencia");

    std::cout << "OK!" << std::endl;
}

void testarTrinca() {
    std::cout << "Testando Trinca... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::Dez, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Dez, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Dez, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Quatro, Naipe::Espada));
    jogo.receberCarta(Carta(Valor::Sete, Naipe::Paus));

    assert(jogo.avaliarMao() == 3);
    assert(jogo.nomeJogada() == "Trinca");

    std::cout << "OK!" << std::endl;
}

void testarDoisPares() {
    std::cout << "Testando Dois Pares... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::Rei, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Rei, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Sete, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Sete, Naipe::Espada));
    jogo.receberCarta(Carta(Valor::Dois, Naipe::Paus));

    assert(jogo.avaliarMao() == 2);
    assert(jogo.nomeJogada() == "Dois Pares");

    std::cout << "OK!" << std::endl;
}

void testarUmPar() {
    std::cout << "Testando Um Par... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::Dama, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Dama, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Cinco, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Sete, Naipe::Espada));
    jogo.receberCarta(Carta(Valor::Nove, Naipe::Paus));

    assert(jogo.avaliarMao() == 1);
    assert(jogo.nomeJogada() == "Um Par");

    std::cout << "OK!" << std::endl;
}

void testarCartaAlta() {
    std::cout << "Testando Carta Alta... ";

    Poker jogo;

    jogo.receberCarta(Carta(Valor::As, Naipe::Paus));
    jogo.receberCarta(Carta(Valor::Rei, Naipe::Copa));
    jogo.receberCarta(Carta(Valor::Oito, Naipe::Ouro));
    jogo.receberCarta(Carta(Valor::Cinco, Naipe::Espada));
    jogo.receberCarta(Carta(Valor::Dois, Naipe::Paus));

    assert(jogo.avaliarMao() == 0);
    assert(jogo.nomeJogada() == "Carta Alta");

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

void testarTrocaDeCartas() {
    std::cout << "Testando troca de cartas... ";

    Baralho baralho;
    baralho.embaralhar();

    Poker jogador;

    jogador.receberCarta(Carta(Valor::As, Naipe::Paus));
    jogador.receberCarta(Carta(Valor::Rei, Naipe::Paus));
    jogador.receberCarta(Carta(Valor::Dama, Naipe::Paus));
    jogador.receberCarta(Carta(Valor::Valete, Naipe::Paus));
    jogador.receberCarta(Carta(Valor::Dez, Naipe::Paus));

    bool trocaValida = jogador.trocarCartas({0, 2, 4}, baralho);
    assert(trocaValida == true);
    assert(jogador.tamanhoMao() == 5);

    bool trocaInvalida = jogador.trocarCartas({0, 1, 2, 3}, baralho);
    assert(trocaInvalida == false);

    bool indiceInvalido = jogador.trocarCartas({5}, baralho);
    assert(indiceInvalido == false);

    std::cout << "OK!" << std::endl;
}

int main() {
    std::cout << "=== INICIANDO TESTES DO POKER ===\\n\\n";

    testarMaoAleatoria();
    testarLimiteDeCartas();

    testarRoyalFlush();
    testarStraightFlush();
    testarQuadra();
    testarFullHouse();
    testarFlush();
    testarSequencia();
    testarSequenciaComAsBaixo();
    testarTrinca();
    testarDoisPares();
    testarUmPar();
    testarCartaAlta();

    testarComparacaoDeMaos();
    testarTrocaDeCartas();

    std::cout << "\\n=====================================\\n";
    std::cout << " TODOS OS TESTES DO POKER PASSARAM!\\n";
    std::cout << "=====================================\\n";

    return 0;
}
