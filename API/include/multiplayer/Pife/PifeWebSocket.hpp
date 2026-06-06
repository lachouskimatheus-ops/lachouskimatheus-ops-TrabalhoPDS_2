#ifndef PIFE_WEBSOCKET_HPP
#define PIFE_WEBSOCKET_HPP

#include <map>
#include <memory>
#include <string>

#include <crow_all.h>

#include "multiplayer/Pife/SalaPife.hpp"

struct SessaoPife {
    std::string idSala;
    int idJogador;
};

class PifeWebSocket {
private:
    static std::map<std::string, std::unique_ptr<SalaPife>> salas_;
    static std::map<crow::websocket::connection*, SessaoPife> sessoes_;

    static SalaPife* obterOuCriarSala(
        const std::string& idSala,
        int maxJogadores
    );

    static void entrarNaSala(
        crow::websocket::connection& conn,
        const crow::json::rvalue& dados
    );

    static void enviarMensagem(
        crow::websocket::connection& conn,
        const crow::json::wvalue& mensagem
    );

    static void enviarEstadoSala(SalaPife* sala);

    static void removerConexao(crow::websocket::connection& conn);

public:
    static void registrar(crow::SimpleApp& app);
};

#endif