/**
 * @file Server.hpp
 * @brief Definição da classe orquestradora do servidor web Crow.
 */

#pragma once
#include "crow_all.h"

/**
 * @class Server
 * @brief Invólucro (Wrapper) sobre o microrframework Crow App.
 *
 * Inicializa a escuta de requisições do sistema e define as configurações base 
 * de execução de rede para as rotas HTTP e WebSockets da coletânea de jogos.
 */
class Server {
private:
    crow::SimpleApp app_; ///< Instância interna do motor do Crow Application.

public:
    /**
     * @brief Configura e inicia o loop de eventos de rede (Event Loop) do servidor.
     * * Bloqueia a thread principal enquanto aguarda conexões de clientes na porta especificada.
     * @param porta Número da porta de rede para escuta (padrão: 18080).
     */
    void iniciar(int porta = 18080);
};