/**
 * @file TrucoWebSocket.hpp
 * @brief Camada WebSocket responsável pela comunicação em tempo real do Truco.
 */

#pragma once

#include <map>
#include <string>

#include "crow_all.h"
#include "Core/Carta.hpp"
#include "coreAPI/SessaoWebSocket.hpp"
#include "multiplayer/Truco/GerenciadorSalasTruco.hpp"
#include "multiplayer/Truco/SalaTruco.hpp"

/**
 * @class TrucoWebSocket
 * @brief Processa conexões, mensagens e ações do Truco via WebSocket.
 */
class TrucoWebSocket {
private:
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    /**
     * @brief Processa a entrada ou reconexão de um jogador em uma sala.
     * @param conexao Conexão WebSocket do jogador.
     * @param dados Dados JSON enviados pelo cliente.
     */
    static void entrarNaSala(crow::websocket::connection& conexao, const crow::json::rvalue& dados);

    /**
     * @brief Processa uma ação de jogo enviada pelo cliente.
     * @param conexao Conexão WebSocket do jogador.
     * @param dados Dados JSON contendo a ação.
     */
    static void processarAcao(crow::websocket::connection& conexao, const crow::json::rvalue& dados);

    /**
     * @brief Responde a uma mensagem de verificação de conexão.
     * @param conexao Conexão WebSocket do cliente.
     */
    static void processarPing(crow::websocket::connection& conexao);

    /**
     * @brief Processa a ação de jogar uma carta.
     * @param conexao Conexão do jogador.
     * @param sala Sala onde ocorre a partida.
     * @param idJogador Identificador do jogador.
     * @param dados Dados contendo o índice da carta.
     */
    static void jogarCarta(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador, const crow::json::rvalue& dados);

    /**
     * @brief Processa um pedido de Truco.
     */
    static void pedirTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Processa a aceitação de um pedido de Truco.
     */
    static void aceitarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Processa a recusa de um pedido de Truco.
     */
    static void recusarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Processa o aumento do valor de um pedido de Truco.
     */
    static void aumentarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Inicia uma nova mão após o encerramento da anterior.
     */
    static void iniciarNovaMao(crow::websocket::connection& conexao, SalaTruco& sala);

    /**
     * @brief Adiciona os dados de uma carta a um objeto JSON.
     * @param destino Objeto JSON de destino.
     * @param carta Carta convertida.
     */
    static void adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta);

    /**
     * @brief Envia uma mensagem JSON ao cliente.
     */
    static void enviarMensagem(crow::websocket::connection& conexao, const crow::json::wvalue& mensagem);

    /**
     * @brief Envia uma mensagem de erro ao cliente.
     */
    static void enviarErro(crow::websocket::connection& conexao, const std::string& mensagem);

    /**
     * @brief Envia o estado atualizado para todos os jogadores da sala.
     */
    static void enviarEstadoSala(SalaTruco* sala);

    /**
     * @brief Envia o estado personalizado para um jogador.
     */
    static void enviarEstadoJogador(SalaTruco* sala, int idJogador, crow::websocket::connection& conexao);

    /**
     * @brief Converte uma fase do Truco em texto.
     */
    static std::string faseParaString(FaseTruco fase);

    /**
     * @brief Converte a variante do Truco em texto.
     */
    static std::string tipoParaString(TipoTruco tipo);

    /**
     * @brief Verifica se um token possui tamanho válido.
     * @param tokenReconexao Token analisado.
     * @return true quando o token for válido; false caso contrário.
     */
    static bool tokenValido(const std::string& tokenReconexao);

    /**
     * @brief Remove uma conexão do controle de sessões.
     */
    static void removerConexao(crow::websocket::connection& conexao);

public:
    /**
     * @brief Registra a rota WebSocket do Truco no servidor Crow.
     * @param app Aplicação principal do Crow.
     */
    static void registrar(crow::SimpleApp& app);
};