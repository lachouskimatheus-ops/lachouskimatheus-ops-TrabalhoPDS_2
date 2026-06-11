/**
 * @file PokerWebSocket.hpp
 * @brief Camada de rede por WebSockets dedicada às ações e eventos do Poker.
 *
 * Escuta os eventos do Crow, processa mensagens recebidas e sincroniza
 * o estado das partidas de Poker com os clientes conectados.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "crow_all.h"

#include "coreAPI/SessaoWebSocket.hpp"
#include "multiplayer/Poker/GerenciadorSalasPoker.hpp"
#include "multiplayer/Poker/SalaPoker.hpp"

/**
 * @class PokerWebSocket
 * @brief Controla as conexões e ações de Poker transmitidas por WebSocket.
 */
class PokerWebSocket {
private:
    /**
     * @brief Associa cada conexão WebSocket à sua sessão ativa.
     */
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    /**
     * @brief Processa a entrada ou reconexão de um jogador em uma sala.
     * @param conexao Conexão WebSocket do jogador.
     * @param dados Dados JSON enviados pelo cliente.
     */
    static void entrarNaSala(crow::websocket::connection& conexao,
                             const crow::json::rvalue& dados);

    /**
     * @brief Processa uma ação de jogo enviada pelo cliente.
     * @param conexao Conexão WebSocket do jogador.
     * @param dados Dados JSON contendo a ação.
     */
    static void processarAcao(crow::websocket::connection& conexao,
                              const crow::json::rvalue& dados);

    /**
     * @brief Responde a uma mensagem de verificação da conexão.
     * @param conexao Conexão WebSocket do cliente.
     */
    static void processarPing(crow::websocket::connection& conexao);

    /**
     * @brief Confirma a troca de cartas de um jogador.
     * @param conexao Conexão WebSocket do jogador.
     * @param sala Sala onde a partida está ocorrendo.
     * @param idJogador Identificador do jogador.
     * @param dados JSON contendo os índices das cartas.
     */
    static void confirmarTroca(crow::websocket::connection& conexao,
                               SalaPoker& sala,
                               int idJogador,
                               const crow::json::rvalue& dados);

    /**
     * @brief Solicita o início de uma nova rodada.
     * @param conexao Conexão WebSocket do solicitante.
     * @param sala Sala onde a partida está ocorrendo.
     */
    static void iniciarNovaRodada(crow::websocket::connection& conexao,
                                  SalaPoker& sala);

    /**
     * @brief Extrai os índices das cartas selecionadas para troca.
     * @param dados JSON recebido do cliente.
     * @param indices Vetor que receberá os índices.
     * @return true se os índices forem válidos; false caso contrário.
     */
    static bool extrairIndicesTroca(const crow::json::rvalue& dados,
                                    std::vector<int>& indices);

    /**
     * @brief Verifica se um token de reconexão possui formato válido.
     * @param tokenReconexao Token que será validado.
     * @return true se o tamanho do token for válido; false caso contrário.
     */
    static bool tokenValido(const std::string& tokenReconexao);

    /**
     * @brief Envia uma mensagem JSON para um cliente.
     * @param conexao Conexão WebSocket de destino.
     * @param mensagem Mensagem que será enviada.
     */
    static void enviarMensagem(crow::websocket::connection& conexao,
                               const json& mensagem);

    /**
     * @brief Envia uma mensagem de erro para um cliente.
     * @param conexao Conexão WebSocket de destino.
     * @param mensagem Descrição do erro.
     */
    static void enviarErro(crow::websocket::connection& conexao,
                           const std::string& mensagem);

    /**
     * @brief Envia o estado atualizado para todos os jogadores da sala.
     * @param sala Sala cujo estado será enviado.
     */
    static void enviarEstadoSala(SalaPoker* sala);

    /**
     * @brief Envia o estado personalizado para um jogador.
     * @param sala Sala da partida.
     * @param idJogador Identificador do jogador.
     * @param conexao Conexão WebSocket do jogador.
     */
    static void enviarEstadoJogador(SalaPoker* sala,
                                    int idJogador,
                                    crow::websocket::connection& conexao);

    /**
     * @brief Remove uma conexão do controle de sessões.
     * @param conexao Conexão que será removida.
     */
    static void removerConexao(crow::websocket::connection& conexao);

public:
    /**
     * @brief Registra a rota WebSocket do Poker no servidor Crow.
     * @param app Aplicação principal do Crow.
     */
    static void registrar(crow::SimpleApp& app);
};
