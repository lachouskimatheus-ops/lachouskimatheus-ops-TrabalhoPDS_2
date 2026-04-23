#ifndef JOGADORPIFE_H
#define JOGADORPIFE_H

#include "cartas.h"
#include <vector>
#include <string.h>

//A ideia é o jogadorPife.h funcionas apenas como um vetor da mão do jogador
//e algumas de suas ações como receberCarta e descartarCarta.

class JogadorPife {
private:
    //pensei primenro em usar Carta mao[9] direto;, mas seria dificil quand precisase add ou remover cartas
    //aí fui ver como fazer com array dinâmico, e tinha essa forma
    std::vector<Carta> mao; //vetor do tipo carta que se chama 'mao'

public:
    void receberCarta(const Carta& carta);
    int tmnhMao() const;
    Carta descartarCarta(int indice); //usar if(c = quantasCartas() >9) : descartar carta -> no regrasPife.cpp dps

    const std::vector<Carta>& verMao() const; //retorna o vetor 'mao', mas sem poder alterar
}
#endif