#include "dependencias/crow_all.h"
#include "dependencias/json.hpp"

#include "../Jogos/Paciencia/include/paciencia.h"
#include "../Jogos/Paciencia/include/regras.h"
#include "../Jogos/Core/CoreBaralho1/cartas.h"
#include "../Jogos/Core/CoreBaralho1/baralho.h"

using json = nlohmann::json;

// ==========================================
// CONVERTE CARTA PARA JSON
// ==========================================
json cartaParaJson(const Carta& c) {
    return json{
        {"valor", (int)c.mostraValor()},
        {"naipe", (int)c.mostraNaipe()}
    };
}

// ==========================================
// CONVERTE ESTADO COMPLETO PARA JSON
// ==========================================
json estadoParaJson(Paciencia& jogo) {
    json j;

    j["pontuacao"]    = jogo.getPontuacao();
    j["recorde"]      = 0;
    j["cava_tamanho"] = jogo.getCavaTamanho();
    j["vitoria"]      = jogo.getVitoria();

    // Cartas escondidas por coluna
    json escondidas = json::array();
    for (int i = 0; i < 7; i++)
        escondidas.push_back(jogo.getCartasEscondidas(i));
    j["cartas_escondidas"] = escondidas;

    // Descarte
    json descJson = json::array();
    for (const auto& c : jogo.getDescarte())
        descJson.push_back(cartaParaJson(c));
    j["descarte"] = descJson;

    // Fundações
    json fundJson = json::array();
    for (const auto& pilha : jogo.getFundacoes()) {
        json p = json::array();
        for (const auto& c : pilha) p.push_back(cartaParaJson(c));
        fundJson.push_back(p);
    }
    j["fundacoes"] = fundJson;

    // Colunas
    json colJson = json::array();
    for (const auto& col : jogo.getColunas()) {
        json c = json::array();
        for (const auto& carta : col) c.push_back(cartaParaJson(carta));
        colJson.push_back(c);
    }
    j["colunas"] = colJson;

    return j;
}

// ==========================================
// MAIN
// ==========================================
int main() {
    crow::SimpleApp app;

    Paciencia jogo;
    std::mutex mtx;
    std::set<crow::websocket::connection*> conexoes;

    auto broadcast = [&](const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto conn : conexoes)
            conn->send_text(msg);
    };

    // ==========================================
    // ROTA WEBSOCKET
    // ==========================================
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([&](crow::websocket::connection& conn) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.insert(&conn);
            }
            std::cout << "Cliente conectado!" << std::endl;
            // Envia estado inicial ao conectar
            conn.send_text(estadoParaJson(jogo).dump());
        })
        .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
            std::lock_guard<std::mutex> lock(mtx);
            conexoes.erase(&conn);
            std::cout << "Cliente desconectado!" << std::endl;
        })
        .onmessage([&](crow::websocket::connection&, const std::string& data, bool) {
            try {
                auto msg = json::parse(data);
                std::string acao = msg["acao"];

                if (acao == "COMPRAR_CARTA") {
                    jogo.comprarCarta();
                }
                else if (acao == "DESFAZER") {
                    jogo.desfazer();
                }
                else if (acao == "NOVO_JOGO") {
                    jogo.iniciarJogo();
                }
                else if (acao == "MOVER") {
                    std::string origemTipoStr  = msg["origem_tipo"];
                    std::string destinoTipoStr = msg["destino_tipo"];
                    int origemIndice           = msg["origem_indice"];
                    int destinoIndice          = msg["destino_indice"];

                    TipoPilha origemTipo  = (origemTipoStr  == "coluna")   ? TipoPilha::Coluna   :
                                           (origemTipoStr  == "descarte") ? TipoPilha::Descarte :
                                                                             TipoPilha::Fundacao;

                    TipoPilha destinoTipo = (destinoTipoStr == "coluna")   ? TipoPilha::Coluna   :
                                           (destinoTipoStr == "descarte") ? TipoPilha::Descarte :
                                                                             TipoPilha::Fundacao;

                    jogo.mover(origemTipo, origemIndice, destinoTipo, destinoIndice);
                }

                // Broadcast do estado atualizado para todos
                broadcast(estadoParaJson(jogo).dump());

            } catch (const std::exception& e) {
                std::cerr << "Erro ao processar mensagem: " << e.what() << std::endl;
            }
        });

    // ==========================================
    // ROTAS HTTP (serve o frontend)
    // ==========================================
    CROW_ROUTE(app, "/")([]{
        crow::response res;
        res.set_static_file_info("frontend/menu.html");
        return res;
    });

    CROW_ROUTE(app, "/<path>")([](const std::string& path){
        crow::response res;
        res.set_static_file_info("frontend/" + path);
        return res;
    });

    std::cout << "Servidor rodando em http://localhost:8080" << std::endl;
    app.port(8080).multithreaded().run();

    return 0;
}