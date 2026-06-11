/**
 * @file PifeWebSocket.hpp
 * @brief Camada de rede por WebSockets dedicada às ações e eventos do jogo Pife.
 *
 * Mapeia os endpoints do Crow para escuta de mensagens bilaterais, decodificando
 * os payloads e transmitindo as atualizações de estado do jogo.
 */

#ifndef PIFE_WEBSOCKET_HPP
#define PIFE_WEBSOCKET_HPP

#include <map>
#include <string>

#include "crow_all.h"

#include "coreAPI/SessaoWebSocket.hpp"
#include "multiplayer/Pife/SalaPife.hpp"

/**
 * @class PifeWebSocket
 * @brief Gerenciador de eventos de rede em tempo real via WebSocket para o Pife.
 *
 * Centraliza funções estáticas para registrar rotas no motor do Crow, tratar ciclos
 * de vida de conexões (abertura, fechamento), validar autenticações e despachar JSONs.
 */
class PifeWebSocket {
private:
    /**
     * @brief Mapeia conexões de baixo nível ativas do Crow para as suas respectivas sessões de contexto.
     */
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    /**
     * @brief Processa a requisição inicial de entrada ou registro de um jogador em uma SalaPife.
     * @param conn Referência para a conexão WebSocket do Crow.
     * @param dados Objeto JSON contendo os parâmetros de entrada (ID da sala, nome, token).
     */
    static void entrarNaSala(crow::websocket::connection& conn, const crow::json::rvalue& dados);

    /**
     * @brief Decodifica as intenções e ações lógicas disparadas pelo cliente (comprar, descartar, bater).
     * @param conn Referência para a conexão WebSocket do Crow.
     * @param dados Objeto JSON descrevendo o tipo de comando e seus argumentos lógicos.
     */
    static void processarAcao(crow::websocket::connection& conn, const crow::json::rvalue& dados);

    /**
     * @brief Trata as requisições de pulso (Heartbeat / Ping) para atestar a estabilidade do canal de rede.
     * @param conn Referência para a conexão WebSocket avaliada.
     */
    static void processarPing(crow::websocket::connection& conn);

    /**
     * @brief Envia uma estrutura genérica formatada do Crow para um cliente específico.
     * @param conn Referência para a conexão de destino.
     * @param mensagem Payload JSON empacotado para transmissão.
     */
    static void enviarMensagem(crow::websocket::connection& conn, const crow::json::wvalue& mensagem);

    /**
     * @brief Fabrica e despacha uma mensagem contendo uma string de erro padronizada para o frontend.
     * @param conn Referência para o canal do cliente.
     * @param mensagem Texto descritivo explicitando o erro ou movimento inválido.
     */
    static void enviarErro(crow::websocket::connection& conn, const std::string& mensagem);

    /**
     * @brief Transmite o estado público global atualizado da sala para todos os integrantes síncronos.
     * @param sala Ponteiro para a sala de Pife alvo.
     */
    static void enviarEstadoSala(SalaPife* sala);

    /**
     * @brief Envia informações exclusivas e privadas de um jogador (ex: cartas da sua mão oculta).
     * @param sala Ponteiro para a sala onde o jogo ocorre.
     * @param idJogador ID numérico identificador do assento do jogador.
     * @param conn Referência para o soquete WebSocket receptor do dado.
     */
    static void enviarEstadoJogador(SalaPife* sala, int idJogador, crow::websocket::connection& conn);

    /**
     * @brief Helper para serializar as propriedades de uma struct Carta para um nó JSON do Crow.
     * @param destino Referência da estrutura JSON do Crow que receberá as chaves de valor e naipe.
     * @param carta Objeto Carta alvo da conversão.
     */
    static void adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta);

    /**
     * @brief Desvincula e expurga uma conexão WebSocket fechada ou caída do mapa de sessões.
     * @param conn Referência para a conexão que está sendo finalizada.
     */
    static void removerConexao(crow::websocket::connection& conn);

    /**
     * @brief Valida se a sintaxe ou assinatura de um token de reconexão fornecido é aceitável.
     * @param tokenReconexao String do token apresentado pelo cliente.
     * @return true se for válido, false caso contrário.
     */
    static bool tokenValido(const std::string& tokenReconexao);

public:
    /**
     * @brief Vincula os ganchos (hooks) de WebSocket do ecossistema do Pife na instância do servidor Crow.
     * @param app Instância orquestradora principal do Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};

#endif