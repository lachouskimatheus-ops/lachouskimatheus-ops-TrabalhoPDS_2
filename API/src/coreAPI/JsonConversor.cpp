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

json JsonConversor::mesaFdpParaJson(const MesaFDP& mesa, int idJogadorSolicitante) {
    json estadoMesa;
    
    // 1. Atributos da Mesa
    estadoMesa["jogador_da_vez_index"] = mesa.getJogadorDaVezIndex();
    estadoMesa["cartas_na_rodada"] = mesa.getCartasNaRodada();
    estadoMesa["total_apostas_rodada"] = mesa.getTotalApostasRodada();
    estadoMesa["jogadores_que_ja_apostaram"] = mesa.getJogadoresQueJaApostaram();
    estadoMesa["aposta_proibida"] = mesa.getApostaProibida();
    
    estadoMesa["carta_vira"] = cartaParaJson(mesa.getCartaVira());
    
    // 3. Cartas na Mesa 
    //Conversão para objetos
    json jsonCartasMesa = json::array();
    for (const Carta* carta : mesa.getCartasNaMesa()) {
        if (carta != nullptr) {
            jsonCartasMesa.push_back(cartaParaJson(*carta));
        }
    }
    estadoMesa["cartas_na_mesa"] = jsonCartasMesa;
    
    // 4. Lógica de Jogadores
    json jsonJogadores = json::array();
    
    for (const auto* j : mesa.getJogadores()) {
        const JogadorFDP* jFDP = dynamic_cast<const JogadorFDP*>(j);
        
        if (jFDP != nullptr) {
            json jJson;
            int idDesteJogador = jFDP->getId();
            
            // Atributos básicos do jogador
            jJson["id"] = idDesteJogador;
            jJson["nome"] = jFDP->getNome();
            jJson["vidas"] = jFDP->getVidas();
            jJson["aposta"] = jFDP->getAposta();
            jJson["vazas"] = jFDP->getVezesGanhas();
            
            //"Anti cheat"
            bool esconderCartas = false;
            
            if (idJogadorSolicitante != -1) {
                if (mesa.getCartasNaRodada() == 1) {
                    // Rodada Cega:
                    if (idDesteJogador == idJogadorSolicitante) esconderCartas = true;
                } else {
                    // Rodada Normal:
                    if (idDesteJogador != idJogadorSolicitante) esconderCartas = true;
                }
            }
            
            
            if (esconderCartas) {
                json maoOculta = json::array();
                for (size_t i = 0; i < jFDP->getMao().size(); ++i) {
                    maoOculta.push_back({
                        {"valor", 0}, 
                        {"naipe", "oculto"} 
                    });
                }
                jJson["mao"] = maoOculta;
            } else {
                // Reaproveitamento das funções acima
                json maoVisivel = json::array();
                for (const Carta* carta : jFDP->getMao()) {
                    if (carta != nullptr) {
                        maoVisivel.push_back(cartaParaJson(*carta));
                    }
                }
                jJson["mao"] = maoVisivel;
            }
            
            jsonJogadores.push_back(jJson);
        }
    }
    
    estadoMesa["jogadores"] = jsonJogadores;
    return estadoMesa;
}