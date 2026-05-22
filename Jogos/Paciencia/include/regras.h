#ifndef REGRAS_H
#define REGRAS_H

#include "baralho.h"
#include <vector>

class Regras {
public:
    // O 'static' permite chamar a função direto pela classe
    static bool podeMoverParaColuna(const Carta& origem, const Carta& destino);
    static bool podeMoverParaFundacao(const Carta& carta, const std::vector<Carta>& pilha);
    static bool podeMoverParaColunaVazia(const Carta& carta);
    
private:
    // Tornamos o construtor privado para ninguém "instanciar" essa classe sem querer
    Regras() {} 
    static bool ehVermelha(const Carta& c);
};

#endif