#include "routes/PokerRoutes.hpp"
#include "coreAPI/Responses.hpp"
#include "multiplayer/Poker/GerenciadorSalasPoker.hpp"
#include "multiplayer/Poker/PokerWebSocket.hpp"

void PokerRoutes::registrar(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/poker/salas")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto dados = crow::json::load(req.body);
        if (!dados) return Responses::erro("JSON inválido");

        std::string modoTexto = dados.has("modo") ? std::string(dados["modo"].s()) : "multiplayer";
        ModoPoker modo;

        if (modoTexto == "computador" || modoTexto == "COMPUTADOR") modo = ModoPoker::ContraComputador;
        else if (modoTexto == "multiplayer" || modoTexto == "MULTIPLAYER") modo = ModoPoker::Multiplayer;
        else return Responses::erro("Modo de jogo inválido");

        int quantidade = modo == ModoPoker::ContraComputador ? 1 :
            (dados.has("quantidade_jogadores") ? dados["quantidade_jogadores"].i() : 2);

        if (modo == ModoPoker::Multiplayer && (quantidade < 2 || quantidade > 4))
            return Responses::erro("A quantidade de jogadores deve estar entre 2 e 4");

        SalaPoker* sala = GerenciadorSalasPoker::criarSala(quantidade, modo);
        if (!sala) return Responses::erro("Não foi possível criar a sala", 500);

        json resultado = {
            {"sala", sala->idSala()},
            {"modo", modo == ModoPoker::ContraComputador ? "COMPUTADOR" : "MULTIPLAYER"},
            {"quantidade_jogadores", sala->maxJogadores()},
            {"link", "/pages/poker.html?sala=" + sala->idSala()}
        };

        return Responses::sucesso("Sala criada com sucesso", resultado);
    });

    PokerWebSocket::registrar(app);
}