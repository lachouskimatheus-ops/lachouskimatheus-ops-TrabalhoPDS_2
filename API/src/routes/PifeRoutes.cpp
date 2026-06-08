#include "routes/PifeRoutes.hpp"

#include "multiplayer/Pife/GerenciadorSalasPife.hpp"
#include "multiplayer/Pife/PifeWebSocket.hpp"

void PifeRoutes::registrar(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/pife/salas")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto dados = crow::json::load(req.body);

        if (!dados) {
            crow::json::wvalue erro;
            erro["sucesso"] = false;
            erro["mensagem"] = "JSON inválido";

            crow::response resposta(400, erro.dump());
            resposta.set_header("Content-Type", "application/json");
            return resposta;
        }

        if (!dados.has("quantidade_jogadores")) {
            crow::json::wvalue erro;
            erro["sucesso"] = false;
            erro["mensagem"] = "Informe a quantidade de jogadores";

            crow::response resposta(400, erro.dump());
            resposta.set_header("Content-Type", "application/json");
            return resposta;
        }

        int quantidadeJogadores = dados["quantidade_jogadores"].i();

        if (quantidadeJogadores < 2 || quantidadeJogadores > 4) {
            crow::json::wvalue erro;
            erro["sucesso"] = false;
            erro["mensagem"] = "A quantidade de jogadores deve estar entre 2 e 4";

            crow::response resposta(400, erro.dump());
            resposta.set_header("Content-Type", "application/json");
            return resposta;
        }

        SalaPife* sala = GerenciadorSalasPife::criarSala(quantidadeJogadores);

        if (sala == nullptr) {
            crow::json::wvalue erro;
            erro["sucesso"] = false;
            erro["mensagem"] = "Não foi possível criar a sala";

            crow::response resposta(500, erro.dump());
            resposta.set_header("Content-Type", "application/json");
            return resposta;
        }

        crow::json::wvalue resultado;
        resultado["sucesso"] = true;
        resultado["sala"] = sala->idSala();
        resultado["quantidade_jogadores"] = sala->maxJogadores();
        resultado["link"] = "/pages/pife.html?sala=" + sala->idSala();

        crow::response resposta(201, resultado.dump());
        resposta.set_header("Content-Type", "application/json");

        return resposta;
    });

    PifeWebSocket::registrar(app);
}