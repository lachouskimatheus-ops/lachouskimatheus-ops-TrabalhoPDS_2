/**
 * @file TrucoWebSocket.hpp
 * @brief Camada de rede por WebSockets dedicada às ações e eventos do Truco.
 *
 * Concentra os ganchos do Crow para comunicação bidirecional em tempo real,
 * processando ações de jogo (jogar carta, pedir/responder truco) e sincronizando o estado.
 */

#pragma once

#include <map>
#include <string>
#include <crow_all.h>
#include "Carta.hpp"
#include "coreAPI/SessaoWebSocket.hpp"
#include "multiplayer/Truco/GerenciadorSalasTruco.hpp"
#include "multiplayer/Truco/SalaTruco.hpp"

/**
 * @class TrucoWebSocket
 * @brief Controlador de eventos de rede em tempo real via WebSocket para o Truco.
 *
 * Mapeia mensagens do frontend, descompacta os payloads recebidos do Crow,
 * delega as validações para a SalaTruco/Engine e devolve o estado serializado.
 */
class TrucoWebSocket {
private:
    /**
     * @brief Associa conexões WebSocket ativas do Crow às suas respetivas instâncias de SessaoWebSocket.
     */
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    /**
     * @brief Processa a requisição inicial de entrada ou registo de um jogador numa SalaTruco.
     * @param conexao Referência para a conexão WebSocket ativa do Crow.
     * @param dados Objeto JSON do Crow contendo as chaves de entrada (idSala, nome, equipe, token).
     */
    static void entrarNaSala(crow::websocket::connection& conexao, const crow::json::rvalue& dados);

    /**
     * @brief Interpeta e despacha as intenções e comandos de jogo disparados pelo cliente.
     * @param conexao Referência para a conexão WebSocket do Crow.
     * @param dados Objeto JSON descrevendo a ação ("jogarCarta", "truco", "aceitar", etc.).
     */
    static void processarAcao(crow::websocket::connection& conexao, const crow::json::rvalue& dados);

    /**
     * @brief Trata pacotes de pulso (Heartbeat / Ping) para atestar a estabilidade da ligação.
     * @param conexao Referência para a conexão WebSocket avaliada.
     */
    static void processarPing(crow::websocket::connection& conexao);

    /**
     * @brief Executa a ação do jogador de lançar uma carta à mesa na rodada atual (queda).
     * @param conexao Referência para o canal de rede do Crow do jogador.
     * @param sala Referência à SalaTruco onde a partida ocorre.
     * @param idJogador ID do assento do jogador que está a descartar.
     * @param dados Objeto JSON contendo o índice da carta a ser jogada.
     */
    static void jogarCarta(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador, const crow::json::rvalue& dados);

    /**
     * @brief Encaminha o pedido de aumento de aposta/pontuação da mão (Truco, Seis, Nove, Doze).
     * @param conexao Referência para o canal de rede do solicitante.
     * @param sala Referência à SalaTruco onde a aposta foi proposta.
     * @param idJogador ID do jogador que pediu truco.
     */
    static void pedirTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Processa a aceitação do aumento da aposta atual por parte da equipe desafiada.
     * @param conexao Referência para o canal de rede do respondente.
     * @param sala Referência à SalaTruco ativa.
     * @param idJogador ID do jogador que aceitou o desafio.
     */
    static void aceitarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Processa a recusa da aposta (fugir do truco), encerrando a mão e dando os pontos à outra equipe.
     * @param conexao Referência para o canal de rede do respondente.
     * @param sala Referência à SalaTruco ativa.
     * @param idJogador ID do jogador que correu/recusou.
     */
    static void recusarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Processa o contra-desafio de aumentar ainda mais o valor do Truco atual (ex: pedir 6 se pediram 3).
     * @param conexao Referência para o canal de rede do jogador que aumentou.
     * @param sala Referência à SalaTruco ativa.
     * @param idJogador ID do jogador que propôs o aumento.
     */
    static void aumentarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);

    /**
     * @brief Comanda a sala para recolher as cartas da mesa e iniciar uma nova distribuição/mão de Truco.
     * @param conexao Referência para o canal do cliente que solicitou o reinício.
     * @param sala Referência à SalaTruco alvo.
     */
    static void iniciarNovaMao(crow::websocket::connection& conexao, SalaTruco& sala);

    /**
     * @brief Helper para converter e formatar uma classe Carta para um nó JSON do Crow (`wvalue`).
     * @param destino Referência da estrutura JSON que receberá as propriedades da carta.
     * @param carta Objeto Carta alvo da conversão.
     */
    static void adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta);

    /**
     * @brief Envia uma estrutura formatada do Crow (`wvalue`) para um cliente específico.
     * @param conexao Referência para a conexão de destino.
     * @param mensagem Payload JSON empacotado para transmissão.
     */
    static void enviarMensagem(crow::websocket::connection& conexao, const crow::json::wvalue& mensagem);

    /**
     * @brief Fabrica e envia uma string de erro padronizada em formato JSON para o frontend.
     * @param conexao Referência para o soquete que receberá o alerta.
     * @param mensagem Texto descritivo explicitando o movimento inválido ou falha de contexto.
     */
    static void enviarErro(crow::websocket::connection& conexao, const std::string& mensagem);

    /**
     * @brief Transmite o estado público global e atualizado da mesa para todos os integrantes síncronos da sala.
     * @param sala Ponteiro para a sala de Truco correspondente.
     */
    static void enviarEstadoSala(SalaTruco* sala);

    /**
     * @brief Envia informações exclusivas e privadas de um assento (ex: cartas ocultas na mão do jogador).
     * @param sala Ponteiro para a sala onde a partida ocorre.
     * @param idJogador ID do assento do jogador consultado.
     * @param conexao Referência para a ligação WebSocket recetora.
     */
    static void enviarEstadoJogador(SalaTruco* sala, int idJogador, crow::websocket::connection& conexao);

    /**
     * @brief Converte o enumerador FaseTruco para uma string correspondente amigável ao JSON.
     * @param fase Valor do enum FaseTruco.
     * @return String com a denominação textual da fase.
     */
    static std::string faseParaString(FaseTruco fase);

    /**
     * @brief Converte o tipo de regulamento de Truco para string.
     * @param tipo Valor do enum TipoTruco (Paulista ou Mineiro).
     * @return String contendo "Paulista" ou "Mineiro".
     */
    static std::string tipoParaString(TipoTruco tipo);

    /**
     * @brief Elimina e expurga uma conexão WebSocket fechada ou caída do mapa de sessões ativas.
     * @param conexao Referência para a ligação do Crow que terminou.
     */
    static void removerConexao(crow::websocket::connection& conexao);

public:
    /**
     * @brief Acopla e regista as rotas de escuta e ganchos de WebSocket do Truco no Crow App.
     * @param app Instância orquestradora global do servidor Crow (SimpleApp).
     */
    static void registrar(crow::SimpleApp& app);
};