#include "BaralhoSujo.hpp"

BaralhoSujo::BaralhoSujo() : Baralho(0) {
    inicializar();
}

void BaralhoSujo::inicializar() {
    this->limpar();

    std::vector<Naipe> naipes = {
        Naipe::paus,
        Naipe::copas,
        Naipe::espadas,
        Naipe::ouros
    };

    for (const Naipe& naipe : naipes) {
        for (int i = 1; i <= 13; i++) {
            if (i == 8 || i == 9 || i == 10) continue;
            cartas_.push_back(new Carta(i, naipe));
        }
    }
}