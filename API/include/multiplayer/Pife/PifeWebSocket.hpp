#ifndef PIFE_WEBSOCKET_HPP
#define PIFE_WEBSOCKET_HPP

#include <map>
#include <string>

#include <crow_all.h>

#include "multiplayer/Pife/SalaPife.hpp"
#include "coreAPI/JogadorConectado.hpp"

struct SessaoPife {
    std::string idSala;
    int idJogador;
};

class PifeWebSocket {
private:
    static std::map<
        crow::websocket::connection*,
        SessaoPife
    > sessoes_;

    static void entrarNaSala(crow::websocket::connection& conn,const crow::json::rvalue& dados);

    static void processarAcao(crow::websocket::connection& conn, const crow::json::rvalue& dados);

    static void enviarMensagem(crow::websocket::connection& conn, const crow::json::wvalue& mensagem);

    static void enviarErro(crow::websocket::connection& conn,const std::string& mensagem);

    static void enviarEstadoSala(SalaPife* sala);

    static void enviarEstadoJogador(SalaPife* sala, int idJogador, crow::websocket::connection& conn);

    static void adicionarCartaAoJson( crow::json::wvalue& destino,const Carta& carta);

    static void removerConexao(crow::websocket::connection& conn);

public:
    static void registrar(crow::SimpleApp& app);
};

#endif