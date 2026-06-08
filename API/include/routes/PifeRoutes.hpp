#pragma once

#include "dependencias/crow_all.h"

//Registra as rotas HTTP para criar sala e a rota WebSocket para o jogo.

class PifeRoutes {
public:
    static void registrar(crow::SimpleApp& app);
};