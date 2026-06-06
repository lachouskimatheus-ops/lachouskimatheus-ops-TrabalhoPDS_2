#include "coreAPI/JsonConversor.hpp"

json JsonConversor::cartaParaJson(const Carta& carta) {
    return json{
        {"valor", static_cast<int>(carta.mostraValor())},
        {"naipe", static_cast<int>(carta.mostraNaipe())}
    };
}

json JsonConversor::maoParaJson(const std::vector<Carta>& mao){
    json j = json::array();

    for(const auto& carta : mao){
        j.push_back(cartaParaJson(carta));
    }
    return j;
}

json JsonConversor::mesaParaJson(const std::vector<Carta>& mesa){
    json j = json::array();

    for(const auto& carta : mesa){
        j.push_back(cartaParaJson(carta));
    }
    return j;
}