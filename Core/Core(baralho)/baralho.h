#ifndef BARALHO_H
#define BARALHO_H
#include <vector>
#include "cartas.h"
 
class Baralho{

private: std::vector<Carta> cartas;

public: 
//Construtor
    Baralho();
    
    void embaralhar();
    Carta retirarCarta();
    int tamanho() const;
    bool estaVazio() const;

};

#endif