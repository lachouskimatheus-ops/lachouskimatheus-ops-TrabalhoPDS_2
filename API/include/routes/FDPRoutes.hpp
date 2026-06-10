/**
 * @file FDPRoutes.hpp
 * @brief Barreira de roteamento de rede para o jogo FDP (Fim de Patrulha / Fudeu).
 *
 * Expõe as rotas HTTP e ganchos de WebSocket necessários para gerenciar as salas,
 * distribuição de cartas, lances de vaza e o sistema de pontuação/placar acumulado do FDP.
 */

#pragma once

#include "crow_all.h"

/**
 * @class FDPRoutes
 * @brief Orquestrador de endpoints HTTP e WebSocket para o ecossistema do jogo FDP.
 *
 * Alinha a infraestrutura do Crow com as chamadas de API feitas pelo cliente, permitindo
 * criar a SalaFDP e propagar eventos síncronos de apostas e vazas.
 */
class FDPRoutes {
public:
    /**
     * @brief Registra os ganchos WebSocket e endpoints HTTP específicos do jogo FDP no Crow.
     * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};