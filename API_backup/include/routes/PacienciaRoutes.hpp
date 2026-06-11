/**
 * @file PacienciaRoutes.hpp
 * @brief Definição das rotas e requisições HTTP para o jogo Paciência (Solitaire).
 *
 * Mapeia os endpoints necessários para criar sessões, registrar jogadas,
 * salvar estados no histórico e lidar com as ações locais do jogo singleplayer.
 */

#pragma once

#include "dependencias/crow_all.h"

/**
 * @class PacienciaRoutes
 * @brief Controlador de endpoints dedicados à lógica e persistência de sessões de Paciência.
 *
 * Como o jogo é tradicionalmente de um jogador, gerencia os estados da partida por meio
 * de requisições REST/HTTP (GET/POST) enviadas pelo cliente.
 */
class PacienciaRoutes {
public:
    /**
     * @brief Registra as rotas REST associadas ao gerenciamento do jogo Paciência.
     * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};