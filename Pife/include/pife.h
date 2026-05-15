#ifndef PIFE_H
#define PIFE_H

#include "baralho.h"
#include "jogadorPife.h"
#include <vector>

//é a implementação do fluxo do jogo, como distribuição das cartas, sequência de jogada de cada jogador...

class Pife {
private:
    Baralho baralho_;
    std::vector<JogadorPife> jogadores_;
    std::vector<Carta> mesa_;
    int jogadorAtual_;

    void distribuirCartas(); //é chamada pelo construtor

public:
    Pife(int quantidadeJogadores); //construtor, ja districui as cartas

    void comprarCarta(); //pega a carta e entraga ao jogadorPife.h
    void colocarNaMesa(int indice); //pega carta que o jogador descartou e coloca na mesa
    void proximoJogador(); //ele não chama o pr

    JogadorPife& consultarJogador(); //retorna o jogador
    const std::vector<Carta>& consultarMesa() const; //retorna a mesa

    //funções para a parte gráfica
    int consultarIndiceJogadorAtual() const;
    int numeroDejogadores() const;
};

#endif