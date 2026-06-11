#pragma once

#include <map>
#include <string>

#include "crow_all.h"
#include "json.hpp"

#include "coreAPI/SessaoWebSocket.hpp"
#include "multiplayer/BlackJack/GerenciadorSalasBlackJack.hpp"

using json = nlohmann::json;

/**
 * @brief Gerencia comunicação WebSocket do Blackjack/21.
 */
class BlackJackWebSocket {
private:
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    static void entrarNaSala(crow::websocket::connection& conexao,
                             const crow::json::rvalue& dados);

    static void processarAcao(crow::websocket::connection& conexao,
                              const crow::json::rvalue& dados);

    static void processarPing(crow::websocket::connection& conexao);

    static void enviarMensagem(crow::websocket::connection& conexao,
                               const json& mensagem);

    static void enviarErro(crow::websocket::connection& conexao,
                           const std::string& mensagem);

    static void enviarEstadoSala(SalaBlackJack* sala);

    static void enviarEstadoJogador(SalaBlackJack* sala,
                                    int idJogador,
                                    crow::websocket::connection& conexao);

    static void removerConexao(crow::websocket::connection& conexao);

    static bool tokenValido(const std::string& tokenReconexao);

public:
    static void registrar(crow::SimpleApp& app);
};
