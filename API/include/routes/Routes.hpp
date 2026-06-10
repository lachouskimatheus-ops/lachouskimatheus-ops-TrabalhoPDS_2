/**
 * @file Routes.hpp
 * @brief Orquestrador global e ponto centralizador de rotas do servidor Crow.
 *
 * Reúne e dispara sequencialmente os submódulos de roteamento de cada jogo 
 * (HTTP e WebSockets) e do frontend estático, acoplando-os à aplicação principal.
 */

#pragma once

#include "dependencias/crow_all.h"

/**
 * @class Routes
 * @brief Classe estática encarregada de centralizar o registro de todas as rotas da API.
 *
 * Atua como um agregador/fachada (FacadePattern) para evitar que o ponto de entrada 
 * principal do servidor (`Server.hpp` ou `main.cpp`) precise incluir diretamente 
 * as dependências de roteamento de cada jogo de forma descentralizada.
 */
class Routes {
public:
    /**
     * @brief Varre e registra todas as rotas HTTP e ganchos WebSocket no ecossistema Crow.
     * * Invoca internamente os métodos estáticos `registrar(app)` de cada subclasse de rotas
     * (ex: FrontendRoutes, PacienciaRoutes, PifeRoutes, etc.).
     * * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrarRotas(crow::SimpleApp& app);
};