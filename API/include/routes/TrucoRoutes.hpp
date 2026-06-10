/**
 * @file TrucoRoutes.hpp
 * @brief Mapeamento de endpoints HTTP de criação e rotas WebSocket do Truco.
 *
 * Configura as portas de entrada para jogadores escolherem as variantes Paulista ou Mineiro
 * e direciona as conexões persistentes para a camada do TrucoWebSocket.
 */

#pragma once

#include <crow_all.h>

/**
 * @class TrucoRoutes
 * @brief Controlador de registro para os pontos de contato de rede do jogo Truco.
 *
 * Gerencia a exposição dos caminhos de rede usados pelo frontend para se conectar
 * às partidas por equipes (2v2), integrando os lobbies às regras de negócio.
 */
class TrucoRoutes {
public:
    /**
     * @brief Acopla os listeners e rotas web específicos do jogo Truco à aplicação principal.
     * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};