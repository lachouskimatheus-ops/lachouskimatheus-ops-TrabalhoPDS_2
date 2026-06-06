#include "dependencias/crow_all.h"
#include "dependencias/json.hpp"

#include "Jogos/Poker/include/poker.h"
#include "Jogos/Core/include/Baralho.hpp"
#include "Jogos/Core/include/Carta.hpp"

#include <iostream>
#include <mutex>
#include <set>
#include <string>

using json = nlohmann::json;

json cartaParaJson(const Carta& c) {
    return json{
        {"valor", (int)c.mostraValor()},
        {"naipe", (int)c.mostraNaipe()},
        {"texto", c.cartaString()}
    };
}

json maoParaJson(const Poker& jogador) {
    json maoJson = json::array();

    const std::vector<Carta>& mao = jogador.verMao();

    for (const auto& carta : mao) {
        maoJson.push_back(cartaParaJson(carta));
    }

    return maoJson;
}

std::string vencedorParaTexto(int resultado) {
    if (resultado == 1) return "Jogador 1";
    if (resultado == -1) return "Jogador 2";
    return "Empate";
}

json estadoParaJson(Poker& jogador1, Poker& jogador2) {
    json estado;

    int resultado = jogador1.compararCom(jogador2);

    estado["jogador1"]["mao"] = maoParaJson(jogador1);
    estado["jogador1"]["jogada"] = jogador1.nomeJogada();
    estado["jogador1"]["forca"] = jogador1.avaliarMao();

    estado["jogador2"]["mao"] = maoParaJson(jogador2);
    estado["jogador2"]["jogada"] = jogador2.nomeJogada();
    estado["jogador2"]["forca"] = jogador2.avaliarMao();

    estado["resultado"] = resultado;
    estado["vencedor"] = vencedorParaTexto(resultado);

    return estado;
}

void novaRodada(Baralho& baralho, Poker& jogador1, Poker& jogador2) {
    baralho.limpar();
    baralho = Baralho();
    baralho.embaralhar();

    jogador1.limparMao();
    jogador2.limparMao();

    for (int i = 0; i < 5; i++) {
        jogador1.receberCarta(baralho.retirarCarta());
        jogador2.receberCarta(baralho.retirarCarta());
    }
}

int main() {
    try {
        crow::SimpleApp app;

        std::cout << "Iniciando servidor do Poker..." << std::endl;

        Baralho baralho;
        Poker jogador1;
        Poker jogador2;

        novaRodada(baralho, jogador1, jogador2);

        std::mutex mtx;
        std::set<crow::websocket::connection*> conexoes;

        auto broadcast = [&](const std::string& msg) {
            std::lock_guard<std::mutex> lock(mtx);

            for (auto conn : conexoes) {
                conn->send_text(msg);
            }
        };

        CROW_WEBSOCKET_ROUTE(app, "/ws/poker")
            .onopen([&](crow::websocket::connection& conn) {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    conexoes.insert(&conn);
                }

                std::cout << "Cliente conectado ao Poker!" << std::endl;

                conn.send_text(estadoParaJson(jogador1, jogador2).dump());
            })
            .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.erase(&conn);

                std::cout << "Cliente desconectado do Poker!" << std::endl;
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool) {
                try {
                    auto msg = json::parse(data);
                    std::string acao = msg["acao"];

                    if (acao == "OBTER_ESTADO_ATUAL") {
                        conn.send_text(estadoParaJson(jogador1, jogador2).dump());
                        return;
                    }

                    if (acao == "NOVA_RODADA" || acao == "NOVO_JOGO") {
                        novaRodada(baralho, jogador1, jogador2);
                    }

                    broadcast(estadoParaJson(jogador1, jogador2).dump());

                } catch (const std::exception& e) {
                    std::cerr << "Erro ao processar mensagem do Poker: " << e.what() << std::endl;
                }
            });

        CROW_ROUTE(app, "/")([]{
            crow::response res;
            res.set_static_file_info("frontend/pages/menu.html");
            return res;
        });

        CROW_ROUTE(app, "/pages/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/pages/" + path);
            return res;
        });

        CROW_ROUTE(app, "/assets/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/assets/" + path);
            return res;
        });

        CROW_ROUTE(app, "/css/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/css/" + path);
            return res;
        });

        CROW_ROUTE(app, "/js/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/js/" + path);
            return res;
        });

        std::cout << "Servidor do Poker rodando em http://localhost:8080" << std::endl;
        std::cout << "WebSocket do Poker em ws://localhost:8080/ws/poker" << std::endl;

        app.port(8080).multithreaded().run();

    } catch (const std::exception& e) {
        std::cerr << "ERRO FATAL: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERRO FATAL DESCONHECIDO" << std::endl;
        return 1;
    }

    return 0;
}