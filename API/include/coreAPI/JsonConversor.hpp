#pragma once
#include <vector>
#include "json.hpp"
#include "Carta.hpp"

using json = nlohmann::json;

class JsonConversor {
public:
    static json cartaParaJson(const Carta& carta);

    //Para o Pife a princípio
    static json maoParaJson(const std::vector<Carta>& mao);
    static json mesaParaJson(const std::vector<Carta>& mesa);
};