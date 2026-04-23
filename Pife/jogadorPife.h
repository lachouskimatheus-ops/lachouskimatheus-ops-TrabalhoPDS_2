#ifndef JOGADORPIFE_H
#define JOGADORPIFE_H

#include "cartas.h"
#include <vector>
#include <string.h>

//A ideia é o jogadorPife.h funcionas apenas como um vetor da mão do jogador
//e algumas de suas ações como receberCarta e descartarCarta.

class JogadorPife {
private:
    std::vector<Carta> mao;

public:
    JogadorPife();

    void receberCarta(const Carta& carta);
    int tmnhMao() const;
    Carta descartarCarta(int indice); //usar if(c = quantasCartas() >9) : descartar carta -> no regrasPife.cpp dps

    const std::vector<Carta>& verMao() const;
};

#endif