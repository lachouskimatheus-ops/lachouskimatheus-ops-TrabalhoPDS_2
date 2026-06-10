/**
 * @file PokerWebSocket.hpp
 * @brief Camada de rede por WebSockets dedicada às ações e eventos do Poker.
 *
 * Escuta os ganchos do Crow, trata o fluxo de mensagens de rede e despacha as
 * atualizações de estado e erros para os clientes conectados nas salas de Poker.
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
 * @brief Controlador de eventos de rede em tempo real via WebSocket para o Poker.
 *
 * Centraliza as rotas do Crow para gerir a entrada de utilizadores, troca de cartas,
 * pulso de sincronização (ping) e a transmissão orientada a objetos do estado da mesa.
 */
class PokerWebSocket {
private:
    /**
     * @brief Associa conexões WebSocket de baixo nível do Crow às suas respetivas instâncias de SessaoWebSocket.
     */
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    /**
     * @brief Processa a requisição inicial de entrada ou registo de um jogador numa SalaPoker.
     * @param conexao Referência para a conexão WebSocket ativa do Crow.
     * @param dados Objeto JSON do Crow contendo os parâmetros (ID da sala, nome do jogador, token).
     */
    static void entrarNaSala(crow::websocket::connection& conexao, const crow::json::rvalue& dados);

    /**
     * @brief Interpeta as intenções e ações lógicas disparadas pelo cliente de Poker.
     * @param conexao Referência para a conexão WebSocket do Crow.
     * @param dados Objeto JSON descrevendo o tipo de comando e os seus argumentos.
     */
    static void processarAcao(crow::websocket::connection& conexao, const crow::json::rvalue& dados);

    /**
     * @brief Trata os pacotes de verificação de atividade (Heartbeat / Ping) para manter a sessão estável.
     * @param conexao Referência para a conexão avaliada.
     */
    static void processarPing(crow::websocket::connection& conexao);

    /**
     * @brief Submete a confirmação de troca de cartas de um jogador específico.
     * @param conexao Referência para o canal de rede do Crow.
     * @param sala Referência à SalaPoker onde a partida ocorre.
     * @param idJogador ID numérico do assento do jogador que está a realizar a ação.
     * @param dados Objeto JSON contendo o array com os índices das cartas a descartar.
     */
    static void confirmarTroca(crow::websocket::connection& conexao, SalaPoker& sala, int idJogador, const crow::json::rvalue& dados);

    /**
     * @brief Solicita o início de uma nova rodada de apostas/distribuição na sala.
     * @param conexao Referência para o canal do cliente que efetuou a requisição.
     * @param sala Referência à SalaPoker alvo.
     */
    static void iniciarNovaRodada(crow::websocket::connection& conexao, SalaPoker& sala);

    /**
     * @brief Extrai e valida um array de inteiros (índices das cartas) contidos no JSON recebido.
     * @param dados Objeto JSON de origem.
     * @param indices Vetor de inteiros que será preenchido com os índices extraídos.
     * @return true se a extração foi bem-sucedida, false caso ocorra alguma anomalia na estrutura.
     */
    static bool extrairIndicesTroca(const crow::json::rvalue& dados, std::vector<int>& indices);

    /**
     * @brief Envia um payload JSON estruturado (usando nlohmann::json) para um cliente.
     * @param conexao Referência para a conexão WebSocket de destino.
     * @param mensagem Estrutura contendo o objeto JSON a ser transmitido.
     */
    static void enviarMensagem(crow::websocket::connection& conexao, const json& mensagem);

    /**
     * @brief Fabrica e envia uma string de erro padronizada em formato JSON para o cliente.
     * @param conexao Referência para a ligação que receberá o alerta.
     * @param mensagem Texto detalhado explicitando o erro ou jogada inválida.
     */
    static void enviarErro(crow::websocket::connection& conexao, const std::string& mensagem);

    /**
     * @brief Transmite o panorama público atualizado da mesa para todos os participantes síncronos da sala.
     * @param sala Ponteiro para a sala de Poker ativa.
     */
    static void enviarEstadoSala(SalaPoker* sala);

    /**
     * @brief Envia dados privados (ex: cartas ocultas da mão) especificamente para o respetivo jogador.
     * @param sala Ponteiro para a sala onde a partida está a decorrer.
     * @param idJogador ID numérico do assento do jogador.
     * @param conexao Referência para a ligação WebSocket recetora.
     */
    static void enviarEstadoJogador(SalaPoker* sala, int idJogador, crow::websocket::connection& conexao);

    /**
     * @brief Desvincula e apaga uma conexão fechada ou instável do histórico de sessões do servidor.
     * @param conexao Referência para a ligação do Crow que terminou.
     */
    static void removerConexao(crow::websocket::connection& conexao);

public:
    /**
     * @brief Acopla e regista as rotas e ganchos (hooks) de WebSocket do Poker na app principal do Crow.
     * @param app Instância orquestradora global do Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};