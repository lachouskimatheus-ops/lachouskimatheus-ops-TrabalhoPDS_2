#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Carta.hpp"

class Jogador_Truco {
private:
    std::string nome;
    std::vector<Carta*> mao; 

public:
    Jogador_Truco(std::string nome);
    
    void receberCarta(Carta* novacarta);
    
    void mostrarmao();
    
    void limparMao();

    Carta* jogarCarta(int indice);

    std::string getNome() const;
};