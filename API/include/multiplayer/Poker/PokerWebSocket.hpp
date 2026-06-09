#pragma once

#include <map>
#include <string>
#include <vector>

#include "crow_all.h"

#include "coreAPI/SessaoWebSocket.hpp"
#include "multiplayer/Poker/GerenciadorSalasPoker.hpp"
#include "multiplayer/Poker/SalaPoker.hpp"

class PokerWebSocket {
private:
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    static void entrarNaSala(crow::websocket::connection& conexao, const crow::json::rvalue& dados);
    static void processarAcao(crow::websocket::connection& conexao, const crow::json::rvalue& dados);
    static void processarPing(crow::websocket::connection& conexao);

    static void confirmarTroca(crow::websocket::connection& conexao, SalaPoker& sala, int idJogador, const crow::json::rvalue& dados);
    static void iniciarNovaRodada(crow::websocket::connection& conexao, SalaPoker& sala);

    static bool extrairIndicesTroca(const crow::json::rvalue& dados, std::vector<int>& indices);

    static void enviarMensagem(crow::websocket::connection& conexao, const json& mensagem);
    static void enviarErro(crow::websocket::connection& conexao, const std::string& mensagem);
    static void enviarEstadoSala(SalaPoker* sala);
    static void enviarEstadoJogador(SalaPoker* sala, int idJogador, crow::websocket::connection& conexao);

    static void removerConexao(crow::websocket::connection& conexao);
    static bool tokenValido(const std::string& tokenReconexao);

public:
    static void registrar(crow::SimpleApp& app);
};