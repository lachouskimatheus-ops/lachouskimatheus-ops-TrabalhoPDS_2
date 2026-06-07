#pragma once

#include <vector>
#include <string>

#include "json.hpp"
#include "Carta.hpp"
#include "MesaFDP.hpp"
#include "JogadorFDP.hpp"

using json = nlohmann::json;

class JsonConversor {
public:
    static json cartaParaJson(const Carta& carta);

    static json maoParaJson(const std::vector<Carta>& mao);
    static json mesaParaJson(const std::vector<Carta>& mesa);

    static json cartaFdpParaJson(const Carta& carta);
    static json mesaFdpParaJson(const MesaFDP& mesa, int idJogadorSolicitante);

private:
    static std::string naipeFdpParaString(Naipe naipe);
};