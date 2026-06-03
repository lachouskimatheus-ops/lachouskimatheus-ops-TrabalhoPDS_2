#include "coreAPI/JsonConversor.hpp"

json JsonConversor::cartaParaJson(
    const Carta& carta
) {

    return {
        {"valor", carta.valorString()},
        {"naipe", carta.naipeString()}
    };
}