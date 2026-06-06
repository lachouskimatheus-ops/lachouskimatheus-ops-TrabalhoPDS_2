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

    j["pontuacao"]    = jogo.getPontuacao();
    j["recorde"]      = 0;
    j["cava_tamanho"] = jogo.getCavaTamanho();
    j["vitoria"]      = jogo.getVitoria();

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

        std::cout << "Iniciando jogo..." << std::endl;
        Paciencia jogo;
        std::cout << "Jogo iniciado!" << std::endl;

        std::mutex mtx;
        std::set<crow::websocket::connection*> conexoes;

        auto broadcast = [&](const std::string& msg) {
            std::lock_guard<std::mutex> lock(mtx);
            for (auto conn : conexoes)
                conn->send_text(msg);
        };

        CROW_WEBSOCKET_ROUTE(app, "/ws")
            .onopen([&](crow::websocket::connection& conn) {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    conexoes.insert(&conn);
                }
                std::cout << "Cliente conectado!" << std::endl;
                conn.send_text(estadoParaJson(jogo).dump());
            })
            .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.erase(&conn);
                std::cout << "Cliente desconectado!" << std::endl;
            })
                        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool) {
                try {
                    auto msg = json::parse(data);
                    std::string acao = msg["acao"];

                    if (acao == "OBTER_ESTADO_ATUAL") {
                        // Apenas envia o estado como está para quem pediu (persistência ao recarregar)
                        conn.send_text(estadoParaJson(jogo).dump());
                        return; // Não precisa de broadcast aqui
                    } 
                    else if (acao == "COMPRAR_CARTA") {
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
                    else if (acao == "MOVER_BLOCO") {
                        int origemCol = msg["origem_coluna"];
                        int cartaIdx  = msg["carta_idx"];
                        int destinoCol = msg["destino_coluna"];
                        jogo.moverBloco(origemCol, cartaIdx, destinoCol);
                    }
                    else if (acao == "MOVER_DA_FUNDACAO") {
                        int fundIdx = msg["fundacao_indice"];
                        int destIdx = msg["destino_indice"];
                        // Na nossa implementação, destinoTipo de fundação é sempre Coluna
                        jogo.moverDaFundacao(fundIdx, TipoPilha::Coluna, destIdx);
                    }

                    // Após qualquer ação, atualiza todos os clientes conectados
                    broadcast(estadoParaJson(jogo).dump());

                } catch (const std::exception& e) {
                    std::cerr << "Erro ao processar mensagem: " << e.what() << std::endl;
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

        std::cout << "Servidor rodando em http://localhost:8080" << std::endl;
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