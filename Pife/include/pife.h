#ifndef PIFE_H
#define PIFE_H

#include "baralho.h"
#include "jogadorPife.h"

//é a implementação do jogo em si, distribuição de cartas sequência de jogada de cada jogador...

class Pife {
private:
    Baralho baralho;
    JogadorPife jogador;

    void distribirCartas();

public:
    Pife();

    void iniciarJogo();
    void comprarCarta();
    void descartarCarta(int indice);
    JogadorPife& consultarJogador();
};

#endif