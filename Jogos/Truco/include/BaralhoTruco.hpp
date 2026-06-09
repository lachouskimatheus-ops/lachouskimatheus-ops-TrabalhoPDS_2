#pragma once

#include "Baralho.hpp"

class BaralhoTruco : public Baralho {
public:
    BaralhoTruco();

    void inicializar();
    Carta* puxarCarta();
};
