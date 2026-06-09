#include "routes/TrucoRoutes.hpp"
#include "multiplayer/Truco/GerenciadorSalasTruco.hpp"
#include "multiplayer/Truco/TrucoWebSocket.hpp"

void TrucoRoutes::registrar(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/truco/salas")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto dados = crow::json::load(req.body);

        if (!dados) {
            return crow::response(
                400,
                R"({"sucesso":false,"mensagem":"JSON inválido"})"
            );
        }

        if (!dados.has("quantidade_jogadores") || !dados.has("tipo")) {
            return crow::response(
                400,
                R"({"sucesso":false,"mensagem":"Informe quantidade_jogadores e tipo"})"
            );
        }

        int quantidade = dados["quantidade_jogadores"].i();
        std::string tipoTexto = dados["tipo"].s();

        if (quantidade != 2 && quantidade != 4) {
            return crow::response(
                400,
                R"({"sucesso":false,"mensagem":"O Truco aceita 2 ou 4 jogadores"})"
            );
        }

        TipoTruco tipo;

        if (tipoTexto == "PAULISTA" || tipoTexto == "paulista") {
            tipo = TipoTruco::Paulista;
        } else if (tipoTexto == "MINEIRO" || tipoTexto == "mineiro") {
            tipo = TipoTruco::Mineiro;
        } else {
            return crow::response(
                400,
                R"({"sucesso":false,"mensagem":"Tipo deve ser PAULISTA ou MINEIRO"})"
            );
        }

        SalaTruco* sala = GerenciadorSalasTruco::criarSala(tipo, quantidade);

        if (sala == nullptr) {
            return crow::response(
                500,
                R"({"sucesso":false,"mensagem":"Não foi possível criar a sala"})"
            );
        }

        crow::json::wvalue resultado;
        resultado["sucesso"] = true;
        resultado["sala"] = sala->idSala();
        resultado["tipo"] = tipo == TipoTruco::Mineiro ? "MINEIRO" : "PAULISTA";
        resultado["quantidade_jogadores"] = quantidade;
        resultado["link"] = "/pages/truco.html?sala=" + sala->idSala();

        crow::response resposta(201, resultado.dump());
        resposta.set_header("Content-Type", "application/json");

        return resposta;
    });

    TrucoWebSocket::registrar(app);
}