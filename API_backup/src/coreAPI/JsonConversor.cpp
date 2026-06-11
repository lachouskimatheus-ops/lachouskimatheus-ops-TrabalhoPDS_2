#include "coreAPI/JsonConversor.hpp"

json JsonConversor::cartaParaJson(const Carta& carta) {
    return json{
        {"valor", static_cast<int>(carta.mostraValor())},
        {"naipe", static_cast<int>(carta.mostraNaipe())}
    };
}

json JsonConversor::maoParaJson(const std::vector<Carta>& mao) {
    json j = json::array();

    for (const auto& carta : mao) {
        j.push_back(cartaParaJson(carta));
    }

    return j;
}

json JsonConversor::mesaParaJson(const std::vector<Carta>& mesa) {
    json j = json::array();

    for (const auto& carta : mesa) {
        j.push_back(cartaParaJson(carta));
    }

    return j;
}

std::string JsonConversor::naipeFdpParaString(Naipe naipe) {
    switch (naipe) {
        case Naipe::paus:
            return "paus";

        case Naipe::copas:
            return "copas";

        case Naipe::espadas:
            return "espadas";

        case Naipe::ouros:
            return "ouros";

        default:
            return "oculto";
    }
}

json JsonConversor::cartaFdpParaJson(const Carta& carta) {
    return json{
        {"valor", carta.getValor()},
        {"naipe", naipeFdpParaString(carta.getNaipe())}
    };
}

json JsonConversor::mesaFdpParaJson(const MesaFDP& mesa, int idJogadorSolicitante) {
    json estadoMesa;

    estadoMesa["jogador_da_vez_index"] = mesa.getJogadorDaVezIndex();
    estadoMesa["cartas_na_rodada"] = mesa.getCartasNaRodada();
    estadoMesa["total_apostas_rodada"] = mesa.getTotalApostasRodada();
    estadoMesa["jogadores_que_ja_apostaram"] = mesa.getJogadoresQueJaApostaram();
    estadoMesa["aposta_proibida"] = mesa.getApostaProibida();

    estadoMesa["carta_vira"] = cartaFdpParaJson(mesa.getCartaVira());

    json jsonCartasMesa = json::array();

    for (const Carta* carta : mesa.getCartasNaMesa()) {
        if (carta != nullptr) {
            jsonCartasMesa.push_back(cartaFdpParaJson(*carta));
        }
    }

    estadoMesa["cartas_na_mesa"] = jsonCartasMesa;

    json jsonJogadores = json::array();

    for (const auto* jogadorBase : mesa.getJogadores()) {
        const JogadorFDP* jogadorFDP = dynamic_cast<const JogadorFDP*>(jogadorBase);

        if (jogadorFDP == nullptr) {
            continue;
        }

        json jogadorJson;

        int idDesteJogador = jogadorFDP->getId();

        jogadorJson["id"] = idDesteJogador;
        jogadorJson["nome"] = jogadorFDP->getNome();
        jogadorJson["name"] = jogadorFDP->getNome();

        jogadorJson["vidas"] = jogadorFDP->getVidas();

        jogadorJson["aposta_atual"] = jogadorFDP->getAposta();
        jogadorJson["vezes_ganhas"] = jogadorFDP->getVezesGanhas();

        jogadorJson["aposta"] = jogadorFDP->getAposta();
        jogadorJson["vazas"] = jogadorFDP->getVezesGanhas();

        json maoJson = json::array();

        bool rodadaCega = mesa.getCartasNaRodada() == 1;
        bool esconderCartas = false;

        if (idJogadorSolicitante >= 0) {
            if (rodadaCega) {
                esconderCartas = (idDesteJogador == idJogadorSolicitante);
            } else {
                esconderCartas = (idDesteJogador != idJogadorSolicitante);
            }
        }

        for (const Carta* carta : jogadorFDP->getMao()) {
            if (carta == nullptr) {
                continue;
            }

            if (esconderCartas) {
                maoJson.push_back({
                    {"oculta", true}
                });
            } else {
                maoJson.push_back(cartaFdpParaJson(*carta));
            }
        }

        jogadorJson["mao"] = maoJson;

        jsonJogadores.push_back(jogadorJson);
    }

    estadoMesa["jogadores"] = jsonJogadores;

    return estadoMesa;
}