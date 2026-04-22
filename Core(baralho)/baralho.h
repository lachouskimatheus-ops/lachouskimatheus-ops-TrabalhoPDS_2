#ifndef BARALHO_H
#define BARALHO_H
#include <vector>
#include "cartas.h"
 
class baralho{

private: std::vector<Carta> cartas;

public: 
//Construtor
    baralho();
    
    void embaralhar();
    Carta retirarCarta();
    int tamanho();
    bool estavazio();

};

#endif