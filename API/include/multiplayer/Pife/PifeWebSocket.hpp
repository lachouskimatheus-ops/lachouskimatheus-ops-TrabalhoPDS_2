/**
 * @file PifeWebSocket.hpp
 * @brief Camada de comunicação WebSocket responsável pelo multiplayer do Pife.
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
 * @brief Gerencia as conexões, mensagens e estados enviados aos jogadores de Pife.
 *
 * A classe registra o endpoint WebSocket do Pife no Crow, interpreta mensagens
 * recebidas do frontend, associa conexões a sessões e envia estados privados
 * personalizados para cada jogador.
 */
class PifeWebSocket {
private:
    /**
     * @brief Mapeia cada conexão WebSocket ativa para sua sessão de jogador.
     */
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    /**
     * @brief Processa a entrada inicial ou a reconexão de um jogador em uma sala.
     * @param conn Referência para a conexão WebSocket do cliente.
     * @param dados Objeto JSON contendo sala, token e nome do jogador.
     */
    static void entrarNaSala(crow::websocket::connection& conn, const crow::json::rvalue& dados);

    /**
     * @brief Processa uma ação enviada por um jogador durante a partida.
     * @param conn Referência para a conexão WebSocket do cliente.
     * @param dados Objeto JSON contendo o nome da ação e seus parâmetros.
     */
    static void processarAcao(crow::websocket::connection& conn, const crow::json::rvalue& dados);

    /**
     * @brief Responde a uma mensagem de verificação de conexão.
     * @param conn Referência para a conexão que enviou o ping.
     */
    static void processarPing(crow::websocket::connection& conn);

    /**
     * @brief Envia um objeto JSON para uma conexão WebSocket.
     * @param conn Referência para a conexão de destino.
     * @param mensagem Objeto JSON que será serializado e enviado.
     */
    static void enviarMensagem(crow::websocket::connection& conn, const crow::json::wvalue& mensagem);

    /**
     * @brief Envia uma mensagem padronizada de erro para o cliente.
     * @param conn Referência para a conexão de destino.
     * @param mensagem Texto descritivo do erro.
     */
    static void enviarErro(crow::websocket::connection& conn, const std::string& mensagem);

    /**
     * @brief Envia o estado atualizado da sala para todos os jogadores conectados.
     * @param sala Ponteiro para a sala que terá o estado transmitido.
     */
    static void enviarEstadoSala(SalaPife* sala);

    /**
     * @brief Envia o estado privado e personalizado para um jogador específico.
     *
     * O estado contém a mão do jogador local e apenas informações públicas
     * sobre os demais participantes.
     *
     * @param sala Ponteiro para a sala do jogador.
     * @param idJogador ID do jogador que receberá o estado.
     * @param conn Referência para a conexão WebSocket de destino.
     */
    static void enviarEstadoJogador(SalaPife* sala, int idJogador, crow::websocket::connection& conn);

    /**
     * @brief Converte uma carta para a representação JSON utilizada pelo frontend.
     * @param destino Objeto JSON que receberá os campos valor e naipe.
     * @param carta Carta que será serializada.
     */
    static void adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta);

    /**
     * @brief Remove uma conexão encerrada do mapa de sessões e da sala.
     * @param conn Referência para a conexão que foi fechada.
     */
    static void removerConexao(crow::websocket::connection& conn);

    /**
     * @brief Valida o tamanho básico de um token de reconexão.
     * @param tokenReconexao Token recebido do cliente.
     * @return true se o tamanho do token estiver dentro dos limites aceitos.
     */
    static bool tokenValido(const std::string& tokenReconexao);

public:
    /**
     * @brief Registra o endpoint WebSocket `/ws/pife` no servidor Crow.
     * @param app Aplicação principal do Crow.
     */
    static void registrar(crow::SimpleApp& app);
};

#endif