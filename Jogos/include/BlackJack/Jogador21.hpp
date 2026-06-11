#pragma once
#include "Jogador.hpp"

class Jogador21 : public Jogador {
private:
    int pontuacaoAtual_; ///< Armazena a soma dos pontos das cartas na mão.

public:
  
    Jogador21(int id, std::string nome);

    ~Jogador21() override = default;

   
    int calcularPontuacao();  
    bool estourou() const;
    int getPontuacao() const;
};
