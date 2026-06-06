#include "routes/FDPRoutes.hpp"

#include "dependencias/json.hpp"

#include "Jogos/FDP/include/BaralhoSujo.hpp"
#include "Jogos/FDP/include/Placar.hpp"
#include "Jogos/FDP/include/MesaFDP.hpp"
#include "Jogos/FDP/include/JogadorFDP.hpp"

#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

using json = nlohmann::json;

namespace {

BaralhoSujo baralho;
Placar placar;
MesaFDP mesa(&baralho, &placar);

std::map<crow::websocket::connection*, int> conexoes;
std::mutex mtx;

bool jogoInicializado = false;

void inicializarJogo() {
    if (jogoInicializado) {
        return;
    }

    mesa.adicionarJogador(new JogadorFDP(0, "Jogador 1"));
    mesa.adicionarJogador(new JogadorFDP(1, "Jogador 2"));
    mesa.adicionarJogador(new JogadorFDP(2, "Jogador 3"));
    mesa.adicionarJogador(new JogadorFDP(3, "Jogador 4"));

    mesa.iniciarPartida();

    jogoInicializado = true;
}

void notificarTodos() {
    std::lock_guard<std::mutex> lock(mtx);

    for (const auto& par : conexoes) {
        crow::websocket::connection* conn = par.first;
        int idJogador = par.second;

        std::string estadoJson = mesa.paraJson(idJogador).dump();

        conn->send_text(estadoJson);
    }
}

}

void FDPRoutes::registrar(crow::SimpleApp& app) {

    inicializarJogo();

    CROW_WEBSOCKET_ROUTE(app, "/ws/fdp")

        .onopen([](crow::websocket::connection& conn) {
            std::cout << "Novo jogador conectado ao FDP." << std::endl;

            {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes[&conn] = -1;
            }

            notificarTodos();
        })

        .onclose([](crow::websocket::connection& conn,
                    const std::string&,
                    uint16_t) {
            std::cout << "Jogador desconectado do FDP." << std::endl;

            std::lock_guard<std::mutex> lock(mtx);
            conexoes.erase(&conn);
        })

        .onmessage([](crow::websocket::connection& conn,
                      const std::string& data,
                      bool) {
            std::cout << "\n========================================" << std::endl;
            std::cout << "[DEBUG FDP] Recebi: " << data << std::endl;

            try {
                auto comando = json::parse(data);

                if (!comando.contains("jogador_id") ||
                    !comando.contains("acao")) {
                    std::cout << "[DEBUG FDP] Mensagem invalida." << std::endl;
                    return;
                }

                int idRemetente = comando["jogador_id"];
                std::string acao = comando["acao"];

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    conexoes[&conn] = idRemetente;
                }

                if (idRemetente != mesa.getJogadorDaVezIndex()) {
                    std::cout << "[SEGURANCA FDP] Jogador "
                              << idRemetente
                              << " tentou jogar fora da vez."
                              << std::endl;
                    return;
                }

                if (acao == "JOGAR_CARTA") {
                    int indiceCarta = comando["indice"];

                    std::cout << "[DEBUG FDP] Jogador "
                              << idRemetente
                              << " tentou jogar carta indice "
                              << indiceCarta
                              << std::endl;

                    if (mesa.jogarCarta(indiceCarta)) {
                        notificarTodos();

                        if (mesa.vazaFinalizada()) {
                            std::cout << "[DEBUG FDP] Vaza finalizada." << std::endl;

                            std::thread([]() {
                                std::this_thread::sleep_for(
                                    std::chrono::milliseconds(2500)
                                );

                                mesa.apurarVencedorDaVaza();

                                if (mesa.rodadaFinalizada()) {
                                    mesa.finalizarRodada();
                                }

                                notificarTodos();
                            }).detach();
                        }
                    }
                }

                else if (acao == "APOSTAR") {
                    int valorAposta = comando["valor"];

                    std::cout << "[DEBUG FDP] Jogador "
                              << idRemetente
                              << " apostou "
                              << valorAposta
                              << std::endl;

                    if (mesa.registrarAposta(valorAposta)) {
                        if (mesa.faseApostasFinalizada()) {
                            mesa.iniciarFaseDeCartas();
                        }

                        notificarTodos();
                    }
                }

                else {
                    std::cout << "[DEBUG FDP] Acao desconhecida: "
                              << acao
                              << std::endl;
                }

            } catch (const std::exception& e) {
                std::cout << "[ERRO FDP] Falha ao processar JSON: "
                          << e.what()
                          << std::endl;
            }
        });
}