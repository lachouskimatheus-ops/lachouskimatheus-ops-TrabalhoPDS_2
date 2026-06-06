#include "routes/FDPRoutes.hpp"
#include "coreAPI/JsonConversor.hpp"

#include "MesaFDP.hpp"
#include "BaralhoSujo.hpp"
#include "Placar.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>

static std::unordered_map<crow::websocket::connection*, int> conexoes_fdp;

static BaralhoSujo baralho_fdp;
static Placar placar_fdp;
static MesaFDP mesa_fdp(&baralho_fdp, &placar_fdp);

void notificarTodosFDP() {
    for (auto const& par : conexoes_fdp) {

        crow::websocket::connection* conn = par.first;
        int idJogador = par.second;

        if (idJogador < 0) {
            continue;
        }

        nlohmann::json jsonSeguro =
            JsonConversor::mesaFdpParaJson(mesa_fdp, idJogador);

        conn->send_text(jsonSeguro.dump());
    }
}

void FDPRoutes::registrar(crow::SimpleApp& app) {

    CROW_WEBSOCKET_ROUTE(app, "/ws/fdp")
    .onopen([](crow::websocket::connection& conn) {
        std::cout << "[FDP] Novo jogador conectado!" << std::endl;

        conexoes_fdp[&conn] = -1;
    })

    .onclose([](crow::websocket::connection& conn, const std::string&, uint16_t) {
        std::cout << "[FDP] Jogador desconectado!" << std::endl;

        conexoes_fdp.erase(&conn);
    })

    .onmessage([](crow::websocket::connection& conn, const std::string& data, bool) {
        try {
            auto comando = nlohmann::json::parse(data);

            if (!comando.contains("jogador_id") || !comando.contains("acao")) {
                return;
            }

            int idRemetente = comando["jogador_id"];
            std::string acao = comando["acao"];

            conexoes_fdp[&conn] = idRemetente;

            if (acao == "ENTRAR") {
                notificarTodosFDP();
                return;
            }

            if (acao == "JOGAR_CARTA") {
                if (!comando.contains("indice")) {
                    return;
                }

                int indiceCarta = comando["indice"];

                if (mesa_fdp.getJogadorDaVez() != nullptr &&
                    idRemetente == mesa_fdp.getJogadorDaVez()->getId()) {

                    if (mesa_fdp.jogarCarta(indiceCarta)) {
                        notificarTodosFDP();

                        if (mesa_fdp.vazaFinalizada()) {
                            std::thread([]() {
                                std::this_thread::sleep_for(std::chrono::milliseconds(2500));

                                mesa_fdp.apurarVencedorDaVaza();

                                if (mesa_fdp.rodadaFinalizada()) {
                                    mesa_fdp.finalizarRodada();
                                }

                                notificarTodosFDP();
                            }).detach();
                        }
                    }
                }
            }

            else if (acao == "APOSTAR") {
                if (!comando.contains("valor")) {
                    return;
                }

                int valorAposta = comando["valor"];

                if (mesa_fdp.registrarAposta(valorAposta)) {
                    if (mesa_fdp.faseApostasFinalizada()) {
                        mesa_fdp.iniciarFaseDeCartas();
                    }

                    notificarTodosFDP();
                }
            }

        } catch (const std::exception& e) {
            std::cout << "[ERRO FDP] " << e.what() << std::endl;
        }
    });
}