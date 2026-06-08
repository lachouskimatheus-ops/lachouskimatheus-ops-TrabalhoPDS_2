#ifndef REGRAS_H
#define REGRAS_H

#include "Baralho.hpp"
#include <vector>

class Regras {
public:
    // Construtor deletado: esta classe não deve ser instanciada
    Regras() = delete;

    static bool podeMoverParaColuna(const Carta& origem, const Carta& destino);
    static bool podeMoverParaFundacao(const Carta& carta, const std::vector<Carta>& pilha);
    static bool podeMoverParaColunaVazia(const Carta& carta);
    
private:
    static bool ehVermelha(const Carta& c);
};

#endif