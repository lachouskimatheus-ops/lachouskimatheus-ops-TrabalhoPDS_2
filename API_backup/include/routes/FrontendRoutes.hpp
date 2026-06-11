/**
 * @file FrontendRoutes.hpp
 * @brief Roteamento e entrega de arquivos estáticos do Frontend.
 *
 * Mapeia as requisições de páginas principais, estilos, scripts e assets visuais
 * necessários para a interface web renderizar os jogos corretamente.
 */

#pragma once

#include "dependencias/crow_all.h"

/**
 * @class FrontendRoutes
 * @brief Gerenciador de endpoints de entrega de páginas HTML e recursos do cliente.
 *
 * Configura as rotas básicas do Crow para servir arquivos públicos (SPA ou páginas independentes)
 * para que os usuários consigam acessar as mesas de jogo a partir de um navegador.
 */
class FrontendRoutes {
public:
    /**
     * @brief Vincula as rotas de fornecimento de conteúdo estático à aplicação Crow.
     * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};