#pragma once

#include "crow_all.h"

/**
 * @brief Centraliza o registro das rotas do Blackjack/21.
 */
class BlackJackRoutes {
public:
    /**
     * @brief Registra as rotas HTTP e o WebSocket do Blackjack/21.
     * @param app Aplicação Crow principal.
     */
    static void registrar(crow::SimpleApp& app);
};
