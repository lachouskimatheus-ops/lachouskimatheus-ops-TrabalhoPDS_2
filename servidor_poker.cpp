#include "dependencias/crow_all.h"
#include "dependencias/json.hpp"

#include "Jogos/Poker/include/poker.h"
#include "Jogos/Core/include/Baralho.hpp"
#include "Jogos/Core/include/Carta.hpp"

#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

using json = nlohmann::json;

int valorNumericoServidor(Valor valor) {
    switch (valor) {
        case Valor::As: return 14;
        case Valor::Dois: return 2;
        case Valor::Tres: return 3;
        case Valor::Quatro: return 4;
        case Valor::Cinco: return 5;
        case Valor::Seis: return 6;
        case Valor::Sete: return 7;
        case Valor::Oito: return 8;
        case Valor::Nove: return 9;
        case Valor::Dez: return 10;
        case Valor::Valete: return 11;
        case Valor::Dama: return 12;
        case Valor::Rei: return 13;
        default: return 0;
    }
}

std::string simboloNaipe(Naipe naipe) {
    if (naipe == Naipe::Copa) return "♥";
    if (naipe == Naipe::Ouro) return "♦";
    if (naipe == Naipe::Espada) return "♠";
    if (naipe == Naipe::Paus) return "♣";
    return "?";
}

std::string corNaipe(Naipe naipe) {
    if (naipe == Naipe::Copa || naipe == Naipe::Ouro) {
        return "vermelha";
    }

    return "preta";
}

json cartaParaJson(const Carta& c) {
    return json{
        {"valor", (int)c.mostraValor()},
        {"naipe", (int)c.mostraNaipe()},
        {"valor_texto", c.valorString()},
        {"naipe_texto", c.naipeString()},
        {"simbolo", simboloNaipe(c.mostraNaipe())},
        {"cor", corNaipe(c.mostraNaipe())},
        {"texto", c.cartaString()},
        {"oculta", false}
    };
}

json cartaOcultaParaJson() {
    return json{
        {"valor", 0},
        {"naipe", 0},
        {"valor_texto", "?"},
        {"naipe_texto", "?"},
        {"simbolo", "🂠"},
        {"cor", "oculta"},
        {"texto", "Carta virada"},
        {"oculta", true}
    };
}

json maoParaJson(const Poker& jogador, bool revelar) {
    json maoJson = json::array();

    const std::vector<Carta>& mao = jogador.verMao();

    for (const auto& carta : mao) {
        if (revelar) {
            maoJson.push_back(cartaParaJson(carta));
        } else {
            maoJson.push_back(cartaOcultaParaJson());
        }
    }

    return maoJson;
}

std::string vencedorParaTexto(int resultado) {
    if (resultado == 1) return "Jogador";
    if (resultado == -1) return "Computador";
    return "Empate";
}

std::vector<int> escolherTrocasComputador(const Poker& computador) {
    std::vector<int> indices;
    int categoria = computador.avaliarMao();

    // Mãos fortes: computador mantém tudo.
    if (categoria >= 4) {
        return indices;
    }

    const std::vector<Carta>& mao = computador.verMao();
    std::vector<int> frequencia(15, 0);

    for (const auto& carta : mao) {
        int valor = valorNumericoServidor(carta.mostraValor());

        if (valor >= 2 && valor <= 14) {
            frequencia[valor]++;
        }
    }

    // Mantém pares/trincas/quadras e troca cartas avulsas.
    for (int i = 0; i < (int)mao.size(); i++) {
        int valor = valorNumericoServidor(mao[i].mostraValor());

        if (frequencia[valor] == 1) {
            indices.push_back(i);
        }
    }

    // Se não tem nada, troca as três cartas mais baixas.
    if ((int)indices.size() == 5) {
        std::vector<std::pair<int, int>> valoresComIndice;

        for (int i = 0; i < (int)mao.size(); i++) {
            valoresComIndice.push_back({
                valorNumericoServidor(mao[i].mostraValor()),
                i
            });
        }

        std::sort(valoresComIndice.begin(), valoresComIndice.end());

        indices.clear();

        for (int i = 0; i < 3; i++) {
            indices.push_back(valoresComIndice[i].second);
        }
    }

    if (indices.size() > 3) {
        indices.resize(3);
    }

    return indices;
}

std::vector<int> lerIndicesTroca(const json& msg) {
    std::vector<int> indices;

    if (!msg.contains("indices") || !msg["indices"].is_array()) {
        return indices;
    }

    for (const auto& item : msg["indices"]) {
        if (item.is_number_integer()) {
            indices.push_back(item.get<int>());
        }
    }

    return indices;
}

json estadoParaJson(
    Poker& jogador,
    Poker& computador,
    const std::string& fase,
    int rodada,
    int pontosJogador,
    int pontosComputador,
    int empates,
    int ultimaTrocaJogador,
    int ultimaTrocaComputador
) {
    json estado;

    bool revelarComputador = (fase == "RESULTADO");
    int resultado = 0;

    estado["fase"] = fase;
    estado["rodada"] = rodada;

    estado["placar"]["jogador"] = pontosJogador;
    estado["placar"]["computador"] = pontosComputador;
    estado["placar"]["empates"] = empates;

    estado["trocas"]["jogador"] = ultimaTrocaJogador;
    estado["trocas"]["computador"] = ultimaTrocaComputador;

    estado["jogador"]["mao"] = maoParaJson(jogador, true);
    estado["jogador"]["jogada"] = jogador.nomeJogada();
    estado["jogador"]["forca"] = jogador.avaliarMao();

    estado["computador"]["mao"] = maoParaJson(computador, revelarComputador);

    if (revelarComputador) {
        resultado = jogador.compararCom(computador);

        estado["computador"]["jogada"] = computador.nomeJogada();
        estado["computador"]["forca"] = computador.avaliarMao();

        estado["resultado"] = resultado;
        estado["vencedor"] = vencedorParaTexto(resultado);

        if (resultado == 1) {
            estado["mensagem"] = "Você venceu a rodada!";
        } else if (resultado == -1) {
            estado["mensagem"] = "O computador venceu a rodada.";
        } else {
            estado["mensagem"] = "A rodada terminou empatada.";
        }
    } else {
        estado["computador"]["jogada"] = "Oculta";
        estado["computador"]["forca"] = -1;

        estado["resultado"] = 0;
        estado["vencedor"] = "Aguardando";
        estado["mensagem"] = "Selecione até 3 cartas para trocar.";
    }

    return estado;
}

void novaRodada(
    Baralho& baralho,
    Poker& jogador,
    Poker& computador,
    std::string& fase,
    int& ultimaTrocaJogador,
    int& ultimaTrocaComputador
) {
    baralho.limpar();

    Baralho novoBaralho;
    novoBaralho.embaralhar();
    baralho = novoBaralho;

    jogador.limparMao();
    computador.limparMao();

    for (int i = 0; i < 5; i++) {
        jogador.receberCarta(baralho.retirarCarta());
        computador.receberCarta(baralho.retirarCarta());
    }

    ultimaTrocaJogador = 0;
    ultimaTrocaComputador = 0;
    fase = "ESCOLHENDO_TROCAS";
}

int main() {
    try {
        crow::SimpleApp app;

        std::cout << "Iniciando servidor do Poker..." << std::endl;

        Baralho baralho;
        Poker jogador;
        Poker computador;

        std::string fase;

        int rodada = 1;
        int pontosJogador = 0;
        int pontosComputador = 0;
        int empates = 0;

        int ultimaTrocaJogador = 0;
        int ultimaTrocaComputador = 0;

        novaRodada(
            baralho,
            jogador,
            computador,
            fase,
            ultimaTrocaJogador,
            ultimaTrocaComputador
        );

        std::mutex mtx;
        std::set<crow::websocket::connection*> conexoes;

        auto montarEstado = [&]() {
            return estadoParaJson(
                jogador,
                computador,
                fase,
                rodada,
                pontosJogador,
                pontosComputador,
                empates,
                ultimaTrocaJogador,
                ultimaTrocaComputador
            ).dump();
        };

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
                conn.send_text(montarEstado());
            })
            .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.erase(&conn);

                std::cout << "Cliente desconectado do Poker!" << std::endl;
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool) {
                try {
                    auto msg = json::parse(data);

                    if (!msg.contains("acao")) {
                        conn.send_text(montarEstado());
                        return;
                    }

                    std::string acao = msg["acao"];

                    if (acao == "OBTER_ESTADO_ATUAL") {
                        conn.send_text(montarEstado());
                        return;
                    }

                    if (acao == "NOVA_RODADA" || acao == "NOVO_JOGO") {
                        rodada++;

                        novaRodada(
                            baralho,
                            jogador,
                            computador,
                            fase,
                            ultimaTrocaJogador,
                            ultimaTrocaComputador
                        );

                        broadcast(montarEstado());
                        return;
                    }

                    if (acao == "TROCAR_CARTAS") {
                        if (fase == "ESCOLHENDO_TROCAS") {
                            std::vector<int> indicesJogador = lerIndicesTroca(msg);

                            jogador.trocarCartas(indicesJogador, baralho);
                            ultimaTrocaJogador = (int)indicesJogador.size();

                            std::vector<int> indicesComputador = escolherTrocasComputador(computador);

                            computador.trocarCartas(indicesComputador, baralho);
                            ultimaTrocaComputador = (int)indicesComputador.size();

                            fase = "RESULTADO";

                            int resultado = jogador.compararCom(computador);

                            if (resultado == 1) {
                                pontosJogador++;
                            } else if (resultado == -1) {
                                pontosComputador++;
                            } else {
                                empates++;
                            }
                        }

                        broadcast(montarEstado());
                        return;
                    }

                    conn.send_text(montarEstado());

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