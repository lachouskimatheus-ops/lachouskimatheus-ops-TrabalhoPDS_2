#pragma once
#include "Juiz.hpp"

class JuizMineiro : public Juiz {
    public:
        int decidirVencedor(std::vector<Carta> cartasNaMesa, Carta vira, bool forcarVencedor = false) override;
};