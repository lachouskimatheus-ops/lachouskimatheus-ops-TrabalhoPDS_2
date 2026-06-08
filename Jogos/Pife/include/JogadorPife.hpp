#ifndef JOGADOR_PIFE_HPP
#define JOGADOR_PIFE_HPP

#include "Carta.hpp"

#include <vector>


class JogadorPife {
private:
    std::vector<Carta> mao_;
    bool indiceValido(int indice) const;

public:
    JogadorPife();
    void receberCarta(const Carta& carta);
    void organizarMao();
    int tmnhMao() const;

    Carta descartarCarta(int indice);
    const std::vector<Carta>& verMao() const;
};

#endif