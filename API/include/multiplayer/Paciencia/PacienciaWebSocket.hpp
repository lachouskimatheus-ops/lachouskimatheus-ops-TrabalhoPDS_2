/**
 * @file PacienciaWebSocket.hpp
 * @brief Definição da camada WebSocket responsável pelas partidas online de Paciência.
 */

#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "crow_all.h"
#include "json.hpp"

#include "Paciencia/Paciencia.hpp"

/**
 * @class PacienciaWebSocket
 * @brief Gerencia as conexões WebSocket e as partidas individuais de Paciência.
 *
 * Cada conexão ativa possui sua própria instância do jogo, permitindo que vários
 * usuários joguem simultaneamente sem compartilhar o estado da partida.
 *
 * Embora a Paciência seja um jogo singleplayer, sua comunicação é realizada
 * online por meio de WebSocket.
 */
class PacienciaWebSocket {
private:
    /**
     * @brief Associa cada conexão WebSocket à sua respectiva partida de Paciência.
     */
    static std::map<crow::websocket::connection*, std::unique_ptr<Paciencia>> partidas_;

    /**
     * @brief Protege o acesso concorrente ao mapa de partidas.
     */
    static std::mutex mutex_;

    /**
     * @brief Converte o estado completo de uma partida para JSON.
     *
     * @param jogo Referência constante para a partida que será serializada.
     * @return Objeto JSON contendo pontuação, cava, descarte, colunas,
     * fundações, cartas escondidas e estado de vitória.
     */
    static nlohmann::json estadoParaJson(const Paciencia& jogo);

    /**
     * @brief Converte a representação textual de uma pilha para o enum TipoPilha.
     *
     * @param tipo Nome da pilha recebido pelo frontend.
     * @return Tipo de pilha correspondente.
     * @throws std::invalid_argument Caso o nome da pilha seja inválido.
     */
    static TipoPilha stringParaTipoPilha(const std::string& tipo);

    /**
     * @brief Processa uma ação enviada pelo cliente.
     *
     * @param conn Conexão WebSocket responsável pela partida.
     * @param dados Objeto JSON contendo a ação e seus parâmetros.
     */
    static void processarAcao(crow::websocket::connection& conn, const nlohmann::json& dados);

    /**
     * @brief Envia o estado atualizado da partida para seu respectivo cliente.
     *
     * @param conn Conexão WebSocket de destino.
     * @param jogo Partida cujo estado será enviado.
     */
    static void enviarEstado(crow::websocket::connection& conn, const Paciencia& jogo);

    /**
     * @brief Envia uma mensagem de erro para uma conexão.
     *
     * @param conn Conexão WebSocket de destino.
     * @param mensagem Descrição textual do erro.
     */
    static void enviarErro(crow::websocket::connection& conn, const std::string& mensagem);

public:
    /**
     * @brief Registra o endpoint WebSocket da Paciência no servidor Crow.
     *
     * O endpoint registrado é `/ws/paciencia`.
     *
     * @param app Instância principal da aplicação Crow.
     */
    static void registrar(crow::SimpleApp& app);
};