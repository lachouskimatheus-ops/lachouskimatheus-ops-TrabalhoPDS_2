#ifndef POKER_H
#define POKER_H

#include "Carta.hpp"
#include <vector>
#include <string>

class Poker {
private:
    std::vector<Carta> mao;

    int valorNumerico(Valor valor) const;
    std::vector<int> contarValores(const std::vector<Carta>& cartas) const;

    bool ehFlush(const std::vector<Carta>& cartas) const;
    bool ehSequencia(const std::vector<Carta>& cartas) const;

public:
    Poker();

    void receberCarta(const Carta& carta);
    void limparMao();

    int tamanhoMao() const;
    const std::vector<Carta>& verMao() const;

    int avaliarMao() const;
    std::string nomeJogada() const;

    int compararCom(const Poker& outro) const;
    std::vector<int> gerarPontuacaoDesempate() const;
};

#endif