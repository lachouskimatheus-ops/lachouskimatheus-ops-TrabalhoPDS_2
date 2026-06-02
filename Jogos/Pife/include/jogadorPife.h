#ifndef JOGADORPIFE_H
#define JOGADORPIFE_H

#include "../../Core/Carta.hpp"
#include <vector>
#include <string>

class JogadorPife {
private:
    std::vector<Carta> mao_;
    bool podeComprarMesa_;

public:
    JogadorPife();

    void receberCarta(const Carta& carta);
    void organizarMao();

    void bloquearCompraMesa();
    void liberarCompraMesa();

    bool podeComprarMesa() const;

    int tmnhMao() const;
    Carta descartarCarta(int indice);

    const std::vector<Carta>& verMao() const;
};

#endif