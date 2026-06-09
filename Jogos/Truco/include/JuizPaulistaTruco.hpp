#pragma once

#include "JuizTruco.hpp"

class JuizPaulistaTruco : public JuizTruco {
private:
    int valorCarta(const Carta& carta, const Carta& vira) const;
    int forcaNaipe(Naipe naipe) const;
    int calcularValorManilha(int valorVira) const;

public:
    int decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor = false) const override;
};