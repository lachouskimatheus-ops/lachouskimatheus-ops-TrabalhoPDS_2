#include "routes/PacienciaRoutes.hpp"
#include "dependencias/json.hpp"
#include "coreAPI/JsonConversor.hpp"
#include "Paciencia/Paciencia.hpp"
#include "Paciencia/Regras.hpp"

#include <iostream>
#include <mutex>
#include <set>
#include <string>

using json = nlohmann::json;

namespace {

json estadoParaJson(Paciencia& jogo) {
    json j;

    // CORRIGIDO: Espaçamento incorreto em 'jogo' removido
    j["pontuacao"]    = jogo.getPontuacao();
    j["recorde"]      = jogo.getRecord();
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
    if (tipo == "coluna") return TipoPilha::Coluna;
    if (tipo == "descarte") return TipoPilha::Descarte;
    return TipoPilha::Fundacao;
}

} // namespace

void PacienciaRoutes::registrar(crow::SimpleApp& app) {
    
    static Paciencia jogo;
    jogo.gerarJogoReversivel();

    static std::mutex mtx;
    static std::set<crow::websocket::connection*> conexoes;

    auto broadcast = [&](const std::string& msg) {
        // O lock guard aqui é excelente para proteger a lista de conexões
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
            conn.send_text(estadoParaJson(jogo).dump());
        })
        .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
            std::lock_guard<std::mutex> lock(mtx);
            conexoes.erase(&conn);
        })
        .onmessage([&](crow::websocket::connection&, const std::string& data, bool) {
            try {
                auto msg = json::parse(data);
                if (!msg.contains("acao")) return;

                std::string acao = msg["acao"];

                // ADICIONADO: Bloqueia o jogo para que duas jogadas não alterem a memória ao mesmo tempo
                std::lock_guard<std::mutex> lock(mtx);

                if (acao == "OBTER_ESTADO_ATUAL") {
                    // Estado enviado no final do escopo por broadcast
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
                    if (msg.contains("origem_tipo") && msg.contains("destino_tipo")) {
                        std::string origemTipoStr = msg["origem_tipo"];
                        std::string destinoTipoStr = msg["destino_tipo"];
                        int origemIndice = msg.value("origem_indice", 0);
                        int destinoIndice = msg.value("destino_indice", 0);

                        // Tratamento preventivo de limites básicos
                        if (origemIndice >= 0 && destinoIndice >= 0) {
                            jogo.mover(
                                stringParaTipoPilha(origemTipoStr), origemIndice,
                                stringParaTipoPilha(destinoTipoStr), destinoIndice
                            );
                        }
                    }
                }
                else if (acao == "MOVER_BLOCO") {
                    if (msg.contains("origem_coluna") && msg.contains("carta_idx") && msg.contains("destino_coluna")) {
                        int origemCol = msg["origem_coluna"];
                        int cartaIdx = msg["carta_idx"];
                        int destinoCol = msg["destino_coluna"];

                        if (origemCol >= 0 && cartaIdx >= 0 && destinoCol >= 0) {
                            jogo.moverBloco(origemCol, cartaIdx, destinoCol);
                        }
                    }
                }
                else if (acao == "MOVER_DA_FUNDACAO") {
                    if (msg.contains("fundacao_indice") && msg.contains("destino_indice")) {
                        int fundIdx = msg["fundacao_indice"];
                        int destIdx = msg["destino_indice"];

                        if (fundIdx >= 0 && destIdx >= 0) {
                            jogo.moverDaFundacao(fundIdx, TipoPilha::Coluna, destIdx);
                        }
                    }
                }
                else if (acao == "MOVER_UMA_PARA_FUNDACAO") {
                    bool moveu = jogo.moverUmaParaFundacao();

                    if (jogo.getVitoria()) {
                        jogo.salvarRecord();
                    }

                    json resposta = estadoParaJson(jogo);
                    resposta["movimento_realizado"] = moveu;
                    
                    // Como já estamos sob o mutex, enviamos de forma segura para os clientes
                    for (auto conn : conexoes) {
                        conn->send_text(resposta.dump());
                    }
                    return;
                }

                // Verifica a vitória de forma unificada para todas as outras ações acima
                if (jogo.getVitoria()) {
                    jogo.salvarRecord();
                }

                // Envia a atualização do tabuleiro para todas as conexões abertas
                for (auto conn : conexoes) {
                    conn->send_text(estadoParaJson(jogo).dump());
                }

            } catch (const std::exception& e) {
                std::cerr << "Erro no processamento de Paciência: " << e.what() << std::endl;
            }
        });
}