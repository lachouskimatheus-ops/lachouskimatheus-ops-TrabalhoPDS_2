#pragma once

#include <string>
#include <vector>
#include "Carta.hpp"

class Poker {
private:
    std::vector<Carta> mao_;

    std::vector<int> gerarPontuacaoDesempate() const;

public:
    Poker() = default;

    bool receberCarta(const Carta& carta);
    void limparMao();

    bool substituirCartas(const std::vector<int>& indices, const std::vector<Carta>& novasCartas);

    int avaliarMao() const;
    int compararCom(const Poker& outraMao) const;
    std::string nomeDaJogada() const;

    int tamanhoMao() const;
    const std::vector<Carta>& verMao() const;
};