#ifndef PIFE_H
#define PIFE_H

#include "baralho.h"
#include "jogadorPife.h"

class Pife {
private:
    Baralho baralho;
    JogadorPife jogador;

public:
    Pife();

    void iniciarJogo();
    void comprarCarta();
    void descartarCarta(int indice);
    JogadorPife& consultarJogador();
};

#endif