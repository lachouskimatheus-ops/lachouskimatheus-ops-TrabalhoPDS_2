#ifndef PIFE_H
#define PIFE_H

#include "Baralho.hpp"
#include "jogadorPife.h"
#include "regrasPife.h"
#include <vector>

class Pife {
private:
    Baralho baralho_;
    std::vector<JogadorPife> jogadores_;
    std::vector<Carta> mesa_;

    int jogadorAtual_;
    bool jogoFinalizado_;

    void distribuirCartas();

public:
    Pife(int quantidadeJogadores);

    void comprarBaralho();
    void comprarMesa();
    void colocarNaMesa(int indice);
    void proximoJogador();

    bool bati();
    bool jogoFinalizado() const;

    JogadorPife& consultarJogador();
    const std::vector<Carta>& consultarMesa() const;

    int consultarIndiceJogadorAtual() const;
    int numeroDejogadores() const;
};

#endif