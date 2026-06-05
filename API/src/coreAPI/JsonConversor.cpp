#include "coreAPI/JsonConversor.hpp"

json JsonConversor::cartaParaJson(const Carta& carta) {
    return json{
        {"valor", static_cast<int>(carta.mostraValor())},
        {"naipe", static_cast<int>(carta.mostraNaipe())}
    };
}