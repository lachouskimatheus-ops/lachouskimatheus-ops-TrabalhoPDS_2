#ifndef BARALHO_H
#define BARALHO_H
#include <vector>
#include "cartas.h"
 
class Baralho{

private: std::vector<Carta> cartas;

public: 
//Construtor
    Baralho(int quantidadeBaralhos = 1); //adicionei esse quantidadeBaralhos,se quiser 2 baralhos, só chamar baralho(2). Aquele =1 é so para iniciação padrão
    
    void embaralhar();
    Carta retirarCarta();
    int tamanho() const;
    bool estaVazio() const;

};

#endif