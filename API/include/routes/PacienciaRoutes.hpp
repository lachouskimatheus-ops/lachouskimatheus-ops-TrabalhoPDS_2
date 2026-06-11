/**
 * @file PacienciaRoutes.hpp
 * @brief Definição do registrador de rotas da Paciência.
 */

#pragma once

#include "crow_all.h"

/**
 * @class PacienciaRoutes
 * @brief Integra as rotas de comunicação da Paciência à aplicação Crow.
 *
 * A classe funciona como ponto de entrada do módulo dentro do registrador
 * geral de rotas da API. A comunicação da partida é delegada à classe
 * PacienciaWebSocket.
 */
class PacienciaRoutes {
public:
    /**
     * @brief Registra as rotas utilizadas pelo jogo Paciência.
     *
     * Atualmente registra o endpoint WebSocket `/ws/paciencia`, responsável
     * pela comunicação em tempo real entre o frontend e o servidor.
     *
     * @param app Instância principal da aplicação Crow.
     */
    static void registrar(crow::SimpleApp& app);
};