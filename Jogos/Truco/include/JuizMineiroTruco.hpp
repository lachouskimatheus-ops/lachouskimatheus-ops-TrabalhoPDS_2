#pragma once

#include "JuizTruco.hpp"

class JuizMineiroTruco : public JuizTruco {
private:
    int valorCarta(const Carta& carta) const;
    int forcaNaipe(Naipe naipe) const;

public:
    int decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor = false) const override;
};