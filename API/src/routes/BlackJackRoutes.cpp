#include "routes/BlackJackRoutes.hpp"

#include "coreAPI/Responses.hpp"
#include "multiplayer/BlackJack/GerenciadorSalasBlackJack.hpp"
#include "multiplayer/BlackJack/BlackJackWebSocket.hpp"

void BlackJackRoutes::registrar(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/blackjack/salas")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto dados = crow::json::load(req.body);

        if (!dados) {
            return Responses::erro("JSON inválido");
        }

        int quantidadeJogadores = 2;

        if (dados.has("quantidade_jogadores")) {
            quantidadeJogadores = dados["quantidade_jogadores"].i();
        }

        if (quantidadeJogadores < 1 || quantidadeJogadores > 4) {
            return Responses::erro(
                "A quantidade de jogadores deve estar entre 1 e 4"
            );
        }

        SalaBlackJack* sala =
            GerenciadorSalasBlackJack::criarSala(quantidadeJogadores);

        if (sala == nullptr) {
            return Responses::erro(
                "Não foi possível criar a sala de Blackjack",
                500
            );
        }

        json resultado = {
            {"sala", sala->idSala()},
            {"quantidade_jogadores", sala->maxJogadores()},
            {"link", "/pages/blackjack.html?sala=" + sala->idSala()}
        };

        return Responses::sucesso(
            "Sala de Blackjack criada com sucesso",
            resultado
        );
    });

    BlackJackWebSocket::registrar(app);
}
