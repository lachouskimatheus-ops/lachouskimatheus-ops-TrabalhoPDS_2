/**
 * @file PifeRoutes.hpp
 * @brief Gerenciador de rotas HTTP e ganchos de WebSocket para o jogo Pife.
 *
 * Estabelece os endpoints públicos para criação de salas no lobby e amarra
 * a rota de comunicação bidirecional em tempo real gerenciada pelo PifeWebSocket.
 */

#pragma once

#include "dependencias/crow_all.h"

/**
 * @class PifeRoutes
 * @brief Responsável por expor a interface de rede (HTTP e WS) para partidas de Pife.
 *
 * Fornece caminhos REST para instanciar novas salas (`/pife/criar`) e canais de
 * comunicação síncronos via WebSocket para a movimentação das cartas de Pife.
 */
class PifeRoutes {
public:
    /**
     * @brief Conecta as rotas HTTP de criação de salas e ganchos WebSocket do Pife ao Crow App.
     * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};