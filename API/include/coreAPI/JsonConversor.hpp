#pragma once

#include "json.hpp"
#include "Carta.hpp"

using json = nlohmann::json;

class JsonConversor {
public:
    static json cartaParaJson(const Carta& carta);
};