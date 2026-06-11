#include "BaralhoTruco.hpp"

BaralhoTruco::BaralhoTruco() : Baralho(0) {
    inicializar();
}

void BaralhoTruco::inicializar() {
    limpar();

    const std::vector<Naipe> naipes = {
        Naipe::paus,
        Naipe::copas,
        Naipe::espadas,
        Naipe::ouros
    };

    for (Naipe naipe : naipes) {
        for (int valor = 1; valor <= 13; ++valor) {
            if (valor == 8 || valor == 9 || valor == 10) continue;
            cartas_.push_back(new Carta(valor, naipe));
        }
    }
}