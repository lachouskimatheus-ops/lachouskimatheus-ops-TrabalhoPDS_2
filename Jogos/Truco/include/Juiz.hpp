#pragma once
#include <vector>
#include "Carta.hpp"

class Juiz{
    public:
    virtual int decidirVencedor(std::vector<Carta*> cartasNaMesa, Carta vira, bool forcarVencedor = false) = 0;

    virtual ~Juiz() = default;
};