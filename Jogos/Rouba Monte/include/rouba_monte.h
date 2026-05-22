#ifndef MONTE_H
#define MONTE_H
#include "../Core/CoreBaralho1/cartas.h"


typedef std::vector<Carta> Monte;

class Jogo {
public:
    // Funções principais que regem as mecânicas
    static void inicializarMesa(Baralho& b, Monte& mesa);
    
    static bool tentaRoubar(Carta comprada, Monte& meuMonte, Monte& monteAdversario);
    
    static bool tentaPegarMesa(Carta comprada, Monte& meuMonte, Monte& mesa);
    
    static void descartar(Carta comprada, Monte& mesa);

    static void exibirEstado(const Monte& mesa, const Monte& m1, const Monte& m2);
};

#endif
