#pragma once

#include <string>
#include <vector>
#include "Carta.hpp"

class Jogador_Truco {
private:
    std::string nome_;
    std::vector<Carta*> mao_;

public:
    explicit Jogador_Truco(const std::string& nome);

    void receberCarta(Carta* novaCarta);
    void limparMao();
    Carta* jogarCarta(int indice);

    const std::string& getNome() const;
    const std::vector<Carta*>& getMao() const;
    int getQuantidadeCartas() const;
};