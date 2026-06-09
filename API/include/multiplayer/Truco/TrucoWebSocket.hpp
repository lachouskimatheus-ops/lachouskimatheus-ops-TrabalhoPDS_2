#pragma once

#include <map>
#include <string>
#include <crow_all.h>
#include "Carta.hpp"
#include "coreAPI/SessaoWebSocket.hpp"
#include "multiplayer/Truco/GerenciadorSalasTruco.hpp"
#include "multiplayer/Truco/SalaTruco.hpp"

class TrucoWebSocket {
private:
    static std::map<crow::websocket::connection*, SessaoWebSocket> sessoes_;

    static void entrarNaSala(crow::websocket::connection& conexao, const crow::json::rvalue& dados);
    static void processarAcao(crow::websocket::connection& conexao, const crow::json::rvalue& dados);
    static void processarPing(crow::websocket::connection& conexao);

    static void jogarCarta(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador, const crow::json::rvalue& dados);
    static void pedirTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);
    static void aceitarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);
    static void recusarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);
    static void aumentarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador);
    static void iniciarNovaMao(crow::websocket::connection& conexao, SalaTruco& sala);

    static void adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta);
    static void enviarMensagem(crow::websocket::connection& conexao, const crow::json::wvalue& mensagem);
    static void enviarErro(crow::websocket::connection& conexao, const std::string& mensagem);
    static void enviarEstadoSala(SalaTruco* sala);
    static void enviarEstadoJogador(SalaTruco* sala, int idJogador, crow::websocket::connection& conexao);

    static std::string faseParaString(FaseTruco fase);
    static std::string tipoParaString(TipoTruco tipo);
    static void removerConexao(crow::websocket::connection& conexao);
    static bool tokenValido(const std::string& tokenReconexao);

public:
    static void registrar(crow::SimpleApp& app);
};