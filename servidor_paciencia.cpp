#include "dependencias/crow_all.h"
#include "dependencias/json.hpp"
#include "Jogos/Paciencia/include/paciencia.h"
#include "Jogos/Paciencia/include/regras.h"
#include "Jogos/Core/Carta.hpp"
#include "Jogos/Core/Baralho.hpp"

using json = nlohmann::json;

json cartaParaJson(const Carta& c) {
    return json{
        {"valor", (int)c.mostraValor()},
        {"naipe", (int)c.mostraNaipe()}
    };
}

json estadoParaJson(Paciencia& jogo) {
    json j;
    j["pontuacao"]      = jogo.getPontuacao();
    j["recorde"]        = 0;
    j["cava_tamanho"]   = jogo.getCavaTamanho();
    j["vitoria"]        = jogo.getVitoria();

    json escondidas = json::array();
    for (int i = 0; i < 7; i++)
        escondidas.push_back(jogo.getCartasEscondidas(i));
    j["cartas_escondidas"] = escondidas;

    json descJson = json::array();
    for (const auto& c : jogo.getDescarte())
        descJson.push_back(cartaParaJson(c));
    j["descarte"] = descJson;

    json fundJson = json::array();
    for (const auto& pilha : jogo.getFundacoes()) {
        json p = json::array();
        for (const auto& c : pilha) p.push_back(cartaParaJson(c));
        fundJson.push_back(p);
    }
    j["fundacoes"] = fundJson;

    json colJson = json::array();
    for (const auto& col : jogo.getColunas()) {
        json c = json::array();
        for (const auto& carta : col) c.push_back(cartaParaJson(carta));
        colJson.push_back(c);
    }
    j["colunas"] = colJson;

    return j;
}

int main() {
    try {
        crow::SimpleApp app;
        Paciencia jogo;
        jogo.gerarJogoReversivel();

        std::mutex mtx;
        std::set<crow::websocket::connection*> conexoes;

        auto broadcast = [&](const std::string& msg) {
            std::lock_guard<std::mutex> lock(mtx);
            for (auto conn : conexoes) conn->send_text(msg);
        };

        CROW_WEBSOCKET_ROUTE(app, "/ws")
            .onopen([&](crow::websocket::connection& conn) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.insert(&conn);
                conn.send_text(estadoParaJson(jogo).dump());
            })
            .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.erase(&conn);
            })
            .onmessage([&](crow::websocket::connection&, const std::string& data, bool) {
                try {
                    auto msg = json::parse(data);
                    std::string acao = msg["acao"];

                    if (acao == "OBTER_ESTADO_ATUAL") {
                        broadcast(estadoParaJson(jogo).dump());
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
                        // Certifique-se de que sua classe Paciencia tenha este método implementado
                        jogo.completarAutomaticamente();
                    }
                    else if (acao == "MOVER") {
                        std::string origemTipoStr = msg["origem_tipo"];
                        std::string destinoTipoStr = msg["destino_tipo"];
                        
                        auto toTipo = [](std::string s) {
                            if (s == "coluna") return TipoPilha::Coluna;
                            if (s == "descarte") return TipoPilha::Descarte;
                            return TipoPilha::Fundacao;
                        };

                        jogo.mover(toTipo(origemTipoStr), (int)msg["origem_indice"], 
                                   toTipo(destinoTipoStr), (int)msg["destino_indice"]);
                    }
                    else if (acao == "MOVER_BLOCO") {
                        jogo.moverBloco((int)msg["origem_coluna"], (int)msg["carta_idx"], (int)msg["destino_coluna"]);
                    }
                    else if (acao == "MOVER_DA_FUNDACAO") {
                        jogo.moverDaFundacao((int)msg["fundacao_indice"], TipoPilha::Coluna, (int)msg["destino_indice"]);
                    }

                    broadcast(estadoParaJson(jogo).dump());

                } catch (const std::exception& e) {
                    std::cerr << "Erro no processamento: " << e.what() << std::endl;
                }
            });

        // Rotas estáticas
        CROW_ROUTE(app, "/")( []{ crow::response res; res.set_static_file_info("frontend/pages/menu.html"); return res; });
        CROW_ROUTE(app, "/pages/<path>")( [](std::string p){ crow::response res; res.set_static_file_info("frontend/pages/" + p); return res; });
        CROW_ROUTE(app, "/assets/<path>")( [](std::string p){ crow::response res; res.set_static_file_info("frontend/assets/" + p); return res; });
        CROW_ROUTE(app, "/css/<path>")( [](std::string p){ crow::response res; res.set_static_file_info("frontend/css/" + p); return res; });
        CROW_ROUTE(app, "/js/<path>")( [](std::string p){ crow::response res; res.set_static_file_info("frontend/js/" + p); return res; });

        app.port(8080).multithreaded().run();
    } catch (const std::exception& e) {
        std::cerr << "ERRO FATAL: " << e.what() << std::endl;
    }
    return 0;
}