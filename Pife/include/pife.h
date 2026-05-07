#ifndef PIFE_H
#define PIFE_H

#include "baralho.h"
#include "jogadorPife.h"

//é a implementação do fluxo do jogo, como distribuição das cartas, sequência de jogada de cada jogador...

class Pife {
private:
    Baralho baralho;
    JogadorPife jogador;

    void distribirCartas(); 

public:
    Pife();

    void comprarCarta(); //compra a carta e entraga ao jogadorPife.h
    void colocarNaMesa(int indice); //pega carta que o jogador descartou e coloca na mesa
    JogadorPife& consultarJogador();
    void próximoJogador(); 
    

};

#endif