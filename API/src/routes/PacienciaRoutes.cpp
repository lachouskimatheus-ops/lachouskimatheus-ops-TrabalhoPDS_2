#include "routes/PacienciaRoutes.hpp"

#include "dependencias/json.hpp"

#include "coreAPI/JsonConversor.hpp"

#include "Jogos/Paciencia/include/Paciencia.hpp"
#include "Jogos/Paciencia/include/Regras.hpp"

#include <iostream>
#include <mutex>
#include <set>
#include <string>

using json = nlohmann::json;

namespace {

json estadoParaJson(Paciencia& jogo) {

    json j;

    j["pontuacao"]    = jogo.getPontuacao();
    j["recorde"]      = 0;
    j["cava_tamanho"] = jogo.getCavaTamanho();
    j["vitoria"]      = jogo.getVitoria();

    json escondidas = json::array();

    for (int i = 0; i < 7; i++) {
        escondidas.push_back(jogo.getCartasEscondidas(i));
    }

    j["cartas_escondidas"] = escondidas;

    json descJson = json::array();

    for (const auto& c : jogo.getDescarte()) {
        descJson.push_back(JsonConversor::cartaParaJson(c));
    }

    j["descarte"] = descJson;

    json fundJson = json::array();

    for (const auto& pilha : jogo.getFundacoes()) {

        json p = json::array();

        for (const auto& c : pilha) {
            p.push_back(JsonConversor::cartaParaJson(c));
        }

        fundJson.push_back(p);
    }

    j["fundacoes"] = fundJson;

    json colJson = json::array();

    for (const auto& col : jogo.getColunas()) {

        json c = json::array();

        for (const auto& carta : col) {
            c.push_back(JsonConversor::cartaParaJson(carta));
        }

        colJson.push_back(c);
    }

    j["colunas"] = colJson;

    return j;
}

TipoPilha stringParaTipoPilha(const std::string& tipo) {

    if (tipo == "coluna") {
        return TipoPilha::Coluna;
    }

    if (tipo == "descarte") {
        return TipoPilha::Descarte;
    }

    return TipoPilha::Fundacao;
}

}

void PacienciaRoutes::registrar(crow::SimpleApp& app) {

    static Paciencia jogo;

    jogo.gerarJogoReversivel();

    static std::mutex mtx;

    static std::set<crow::websocket::connection*> conexoes;

    auto broadcast = [&](const std::string& msg) {

        std::lock_guard<std::mutex> lock(mtx);

        for (auto conn : conexoes) {
            conn->send_text(msg);
        }
    };

    CROW_WEBSOCKET_ROUTE(app, "/ws")

        .onopen([&](crow::websocket::connection& conn) {

            {
                std::lock_guard<std::mutex> lock(mtx);

                conexoes.insert(&conn);
            }

            conn.send_text(
                estadoParaJson(jogo).dump()
            );
        })

        .onclose([&](crow::websocket::connection& conn,
                     const std::string&,
                     uint16_t) {

            std::lock_guard<std::mutex> lock(mtx);

            conexoes.erase(&conn);
        })

        .onmessage([&](crow::websocket::connection&,
                       const std::string& data,
                       bool) {

            try {

                auto msg = json::parse(data);

                std::string acao = msg["acao"];

                if (acao == "OBTER_ESTADO_ATUAL") {

                    broadcast(
                        estadoParaJson(jogo).dump()
                    );
                }

                else if (acao == "COMPRAR_CARTA") {

                    jogo.comprarCarta();
                }

                else if (acao == "DESFAZER") {

                    jogo.desfazer();
                }

                else if (acao == "NOVO_JOGO") {

                    jogo.gerarJogoReversivel();
                }

                else if (acao == "COMPLETAR_AUTOMATICAMENTE") {

                    jogo.completarAutomaticamente();
                }

                else if (acao == "MOVER") {

                    std::string origemTipoStr =
                        msg["origem_tipo"];

                    std::string destinoTipoStr =
                        msg["destino_tipo"];

                    int origemIndice =
                        msg["origem_indice"];

                    int destinoIndice =
                        msg["destino_indice"];

                    jogo.mover(
                        stringParaTipoPilha(origemTipoStr),
                        origemIndice,
                        stringParaTipoPilha(destinoTipoStr),
                        destinoIndice
                    );
                }

                else if (acao == "MOVER_BLOCO") {

                    int origemCol =
                        msg["origem_coluna"];

                    int cartaIdx =
                        msg["carta_idx"];

                    int destinoCol =
                        msg["destino_coluna"];

                    jogo.moverBloco(
                        origemCol,
                        cartaIdx,
                        destinoCol
                    );
                }

                else if (acao == "MOVER_DA_FUNDACAO") {

                    int fundIdx =
                        msg["fundacao_indice"];

                    int destIdx =
                        msg["destino_indice"];

                    jogo.moverDaFundacao(
                        fundIdx,
                        TipoPilha::Coluna,
                        destIdx
                    );
                }

                else if (acao == "MOVER_UMA_PARA_FUNDACAO") {

                    bool moveu =
                        jogo.moverUmaParaFundacao();

                    json resposta =
                        estadoParaJson(jogo);

                    resposta["movimento_realizado"] =
                        moveu;

                    broadcast(
                        resposta.dump()
                    );

                    return;
                }

                broadcast(
                    estadoParaJson(jogo).dump()
                );

            } catch (const std::exception& e) {

                std::cerr
                    << "Erro no processamento: "
                    << e.what()
                    << std::endl;
            }
        });
}