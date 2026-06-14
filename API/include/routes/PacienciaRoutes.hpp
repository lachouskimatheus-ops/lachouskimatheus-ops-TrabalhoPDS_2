/**
 * @file PacienciaRoutes.hpp
 * @brief Definição do registrador de rotas da Paciência.
 */

#pragma once

#include "crow_all.h"

/**
 * @class PacienciaRoutes
 * @brief Integra a comunicação da Paciência à aplicação Crow.
 *
 * Registra o endpoint WebSocket utilizado pelo frontend para enviar
 * movimentos e receber o estado atualizado da partida.
 */
class PacienciaRoutes {
public:
    /**
     * @brief Registra o WebSocket `/ws/paciencia`.
     * @param app Instância principal da aplicação Crow.
     */
    static void registrar(crow::SimpleApp& app);
};