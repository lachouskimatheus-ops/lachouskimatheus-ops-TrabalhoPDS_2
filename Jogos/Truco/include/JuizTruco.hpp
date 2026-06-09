#pragma once

#include <vector>
#include "Carta.hpp"

class JuizTruco {
public:
    virtual ~JuizTruco() = default;

    virtual int decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor = false) const = 0;
};