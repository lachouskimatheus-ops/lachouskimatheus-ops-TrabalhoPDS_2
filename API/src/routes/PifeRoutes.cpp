#include "routes/PifeRoutes.hpp"
#include "dependencias/json.hpp"
#include "coreAPI/JsonConversor.hpp"
#include "Jogos/Pife/include/pife.h"

#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <string>

using json = nlohmann::json;

namespace {

struct SalaPife {
    Pife jogo;
    std::set<crow::websocket::connection*> conexoes;

    SalaPife() : jogo(2) {}
};

std::mutex mtx;

std::map<std::string, SalaPife> salas;

std::map<crow::websocket::connection*, std::string> conexaoParaSala;
std::map<crow::websocket::connection*, int> conexaoParaJogador;

json estadoParaJson(Pife& jogo, int jogadorLocal) {
    json j;

    j["jogador_local"] = jogadorLocal;
    j["jogador_atual"] = jogo.consultarIndiceJogadorAtual();
    j["numero_jogadores"] = jogo.numeroDeJogadores();
    j["jogo_finalizado"] = jogo.jogoFinalizado();

    j["mesa"] = JsonConversor::mesaParaJson(jogo.consultarMesa());

    j["minha_mao"] = JsonConversor::maoParaJson(
        jogo.consultarJogador().verMao()
    );

    return j;
}

void enviarEstadoParaSala(const std::string& salaId) {
    auto& sala = salas.at(salaId);

    for (auto conn : sala.conexoes) {
        int jogadorLocal = conexaoParaJogador[conn];

        conn->send_text(
            estadoParaJson(sala.jogo, jogadorLocal).dump()
        );
    }
}

bool ehTurnoDoJogador(Pife& jogo, int jogador) {
    return jogo.consultarIndiceJogadorAtual() == jogador;
}

}

void PifeRoutes::registrar(crow::SimpleApp& app) {

    CROW_WEBSOCKET_ROUTE(app, "/ws/pife")
        .onopen([&](crow::websocket::connection& conn) {
            std::cout << "Cliente conectado ao Pife." << std::endl;
        })

        .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
            std::lock_guard<std::mutex> lock(mtx);

            if (conexaoParaSala.count(&conn)) {
                std::string salaId = conexaoParaSala[&conn];

                if (salas.count(salaId)) {
                    salas[salaId].conexoes.erase(&conn);
                }

                conexaoParaSala.erase(&conn);
                conexaoParaJogador.erase(&conn);
            }

            std::cout << "Cliente desconectado do Pife." << std::endl;
        })

        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool) {
            try {
                auto msg = json::parse(data);

                std::string acao = msg["acao"];

                std::lock_guard<std::mutex> lock(mtx);

                if (acao == "ENTRAR_SALA") {
                    std::string salaId = msg.value("sala_id", "global");

                    if (!salas.count(salaId)) {
                        salas.emplace(salaId, SalaPife());
                    }

                    int jogador = 0;

                    if (msg.contains("jogador")) {
                        jogador = msg["jogador"];
                    } else {
                        jogador = static_cast<int>(salas[salaId].conexoes.size());
                    }

                    if (jogador < 0) {
                        jogador = 0;
                    }

                    if (jogador > 1) {
                        jogador = 1;
                    }

                    salas[salaId].conexoes.insert(&conn);
                    conexaoParaSala[&conn] = salaId;
                    conexaoParaJogador[&conn] = jogador;

                    conn.send_text(
                        estadoParaJson(salas[salaId].jogo, jogador).dump()
                    );

                    return;
                }

                if (!conexaoParaSala.count(&conn)) {
                    conn.send_text(json{
                        {"erro", "Cliente ainda não entrou em uma sala."}
                    }.dump());

                    return;
                }

                std::string salaId = conexaoParaSala[&conn];
                int jogador = conexaoParaJogador[&conn];

                SalaPife& sala = salas[salaId];
                Pife& jogo = sala.jogo;

                if (acao == "OBTER_ESTADO_ATUAL") {
                    conn.send_text(
                        estadoParaJson(jogo, jogador).dump()
                    );

                    return;
                }

                if (!ehTurnoDoJogador(jogo, jogador)) {
                    conn.send_text(json{
                        {"erro", "Não é seu turno."}
                    }.dump());

                    return;
                }

                if (acao == "COMPRAR_BARALHO") {
                    jogo.comprarBaralho();
                }

                else if (acao == "COMPRAR_MESA") {
                    jogo.comprarMesa();
                }

                else if (acao == "DESCARTAR") {
                    int indice = msg["indice"];
                    jogo.colocarNaMesa(indice);
                    jogo.proximoJogador();
                }

                else if (acao == "BATER") {
                    jogo.bati();
                }

                else if (acao == "ORGANIZAR") {
                    jogo.consultarJogador().organizarMao();
                }

                else if (acao == "NOVO_JOGO") {
                    salas[salaId] = SalaPife();
                }

                enviarEstadoParaSala(salaId);

            } catch (const std::exception& e) {
                std::cerr << "Erro no PifeRoutes: "
                          << e.what()
                          << std::endl;
            }
        });
}