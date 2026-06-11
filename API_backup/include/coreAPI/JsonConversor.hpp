/**
 * @file JsonConversor.hpp
 * @brief Classe utilitária para serialização e conversão de objetos do jogo para JSON.
 */

#pragma once

#include <vector>
#include <string>

#include "json.hpp"
#include "Carta.hpp"
#include "MesaFDP.hpp"
#include "JogadorFDP.hpp"

using json = nlohmann::json;

/**
 * @class JsonConversor
 * @brief Centraliza métodos estáticos de conversão para o formato JSON (`nlohmann::json`).
 *
 * Transforma estruturas nativas do C++ (como cartas, vetores e estados de mesas)
 * em objetos textuais JSON prontos para serem transmitidos via WebSockets ou rotas HTTP.
 */
class JsonConversor {
public:
    /**
     * @brief Serializa um objeto Carta genérico para JSON.
     * @param carta Referência constante para a Carta avaliada.
     * @return Objeto JSON com as propriedades de valor e naipe.
     */
    static json cartaParaJson(const Carta& carta);

    /**
     * @brief Converte um vetor de cartas (mão de um jogador) para uma lista JSON.
     * @param mao Vetor contendo os objetos Carta.
     * @return Objeto JSON do tipo array (`[...]`).
     */
    static json maoParaJson(const std::vector<Carta>& mao);

    /**
     * @brief Converte as cartas comunitárias expostas na mesa para formato JSON.
     * @param mesa Vetor contendo as cartas visíveis no centro da mesa.
     * @return Objeto JSON do tipo array com as cartas.
     */
    static json mesaParaJson(const std::vector<Carta>& mesa);

    /**
     * @brief Serializa uma carta aplicando as regras e nomenclaturas específicas do jogo FDP.
     * @param carta Referência constante para a Carta.
     * @return Objeto JSON formatado para o ecossistema FDP.
     */
    static json cartaFdpParaJson(const Carta& carta);

    /**
     * @brief Serializa o estado completo da MesaFDP adaptando a visibilidade conforme o jogador solicitante.
     * * Esconde cartas ocultas de adversários para evitar trapaças através da inspeção do tráfego de rede.
     * @param mesa Referência constante para a mesa de FDP ativa.
     * @param idJogadorSolicitante ID do jogador que está a pedir a atualização da tela.
     * @return Objeto JSON contendo o snapshot seguro da mesa.
     */
    static json mesaFdpParaJson(const MesaFDP& mesa, int idJogadorSolicitante);

private:
    /**
     * @brief Traduz o enum de Naipe para uma string específica aceita pelo frontend do FDP.
     * @param naipe Enum do Naipe a ser convertido.
     * @return String contendo o nome textual equivalente do naipe.
     */
    static std::string naipeFdpParaString(Naipe naipe);
};