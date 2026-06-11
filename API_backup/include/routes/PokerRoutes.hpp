/**
 * @file PokerRoutes.hpp
 * @brief Roteamento de rede dedicado às ações e gerenciamento do jogo Poker.
 *
 * Mapeia os pontos de acesso HTTP para inicializar mesas em modos locais (contra bot)
 * ou multiplayer, além de amarrar o tráfego em tempo real ao PokerWebSocket.
 */

#pragma once

#include "crow_all.h"

/**
 * @class PokerRoutes
 * @brief Centraliza o mapeamento de endpoints e rotas ativas do Poker.
 *
 * Divide o escopo de chamadas do servidor para comportar tanto o fluxo REST 
 * (configurações do lobby e modo de jogo) quanto o canal persistente via WebSocket.
 */
class PokerRoutes {
public:
    /**
     * @brief Configura e injeta as rotas e ganchos do Poker no servidor Crow.
     * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};