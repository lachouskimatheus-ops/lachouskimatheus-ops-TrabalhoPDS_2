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
    json estado;

    estado["pontuacao"] = jogo.getPontuacao();
    estado["recorde"] = jogo.getRecord();
    estado["cava_tamanho"] = jogo.getCavaTamanho();
    estado["vitoria"] = jogo.getVitoria();

    estado["cartas_escondidas"] = json::array();
    for (int i = 0; i < 7; i++)
        estado["cartas_escondidas"].push_back(jogo.getCartasEscondidas(i));

    estado["descarte"] = json::array();
    for (const Carta& carta : jogo.getDescarte())
        estado["descarte"].push_back(JsonConversor::cartaParaJson(carta));

    estado["fundacoes"] = json::array();
    for (const auto& pilha : jogo.getFundacoes()) {
        json fundacao = json::array();

        for (const Carta& carta : pilha)
            fundacao.push_back(JsonConversor::cartaParaJson(carta));

        estado["fundacoes"].push_back(fundacao);
    }

    estado["colunas"] = json::array();
    for (const auto& pilha : jogo.getColunas()) {
        json coluna = json::array();

        for (const Carta& carta : pilha)
            coluna.push_back(JsonConversor::cartaParaJson(carta));

        estado["colunas"].push_back(coluna);
    }

    return estado;
}

TipoPilha stringParaTipoPilha(const std::string& tipo) {
    if (tipo == "coluna") return TipoPilha::Coluna;
    if (tipo == "descarte") return TipoPilha::Descarte;
    return TipoPilha::Fundacao;
}

void enviarErro(crow::websocket::connection& conn, const std::string& mensagem) {
    json resposta;
    resposta["tipo"] = "erro";
    resposta["erro"] = mensagem;
    resposta["mensagem"] = mensagem;
    conn.send_text(resposta.dump());
}

} // namespace

void PacienciaRoutes::registrar(crow::SimpleApp& app) {
    static Paciencia jogo;
    static std::mutex mutex;
    static std::set<crow::websocket::connection*> conexoes;
    static bool jogoInicializado = false;

    if (!jogoInicializado) {
        jogo.gerarJogoReversivel();
        jogoInicializado = true;
    }

    CROW_WEBSOCKET_ROUTE(app, "/ws/paciencia")
    .onopen([](crow::websocket::connection& conn) {
        std::string estado;

        {
            std::lock_guard<std::mutex> lock(mutex);
            conexoes.insert(&conn);
            estado = estadoParaJson(jogo).dump();
        }

        conn.send_text(estado);
    })
    .onclose([](crow::websocket::connection& conn, const std::string&, uint16_t) {
        std::lock_guard<std::mutex> lock(mutex);
        conexoes.erase(&conn);
    })
    .onmessage([](crow::websocket::connection& conn, const std::string& data, bool binario) {
        if (binario) {
            enviarErro(conn, "Mensagens binárias não são aceitas");
            return;
        }

        try {
            json mensagem = json::parse(data);

            if (!mensagem.contains("acao") || !mensagem["acao"].is_string()) {
                enviarErro(conn, "A mensagem não possui uma ação válida");
                return;
            }

            const std::string acao = mensagem["acao"];

            if (acao == "PING") {
                json resposta;
                resposta["tipo"] = "pong";
                conn.send_text(resposta.dump());
                return;
            }

            std::string respostaTexto;
            std::set<crow::websocket::connection*> destinatarios;

            {
                std::lock_guard<std::mutex> lock(mutex);

                if (acao == "OBTER_ESTADO_ATUAL") {
                    respostaTexto = estadoParaJson(jogo).dump();
                    destinatarios.insert(&conn);
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
                    if (!mensagem.contains("origem_tipo") ||
                        !mensagem.contains("destino_tipo")) {
                        throw std::invalid_argument("Dados do movimento incompletos");
                    }

                    std::string origemTipo = mensagem["origem_tipo"];
                    std::string destinoTipo = mensagem["destino_tipo"];
                    int origemIndice = mensagem.value("origem_indice", 0);
                    int destinoIndice = mensagem.value("destino_indice", 0);

                    if (origemIndice < 0 || destinoIndice < 0)
                        throw std::out_of_range("Índice de movimento inválido");

                    jogo.mover(
                        stringParaTipoPilha(origemTipo),
                        origemIndice,
                        stringParaTipoPilha(destinoTipo),
                        destinoIndice
                    );
                }
                else if (acao == "MOVER_BLOCO") {
                    if (!mensagem.contains("origem_coluna") ||
                        !mensagem.contains("carta_idx") ||
                        !mensagem.contains("destino_coluna")) {
                        throw std::invalid_argument("Dados do bloco incompletos");
                    }

                    int origem = mensagem["origem_coluna"];
                    int carta = mensagem["carta_idx"];
                    int destino = mensagem["destino_coluna"];

                    if (origem < 0 || carta < 0 || destino < 0)
                        throw std::out_of_range("Índice de bloco inválido");

                    jogo.moverBloco(origem, carta, destino);
                }
                else if (acao == "MOVER_DA_FUNDACAO") {
                    if (!mensagem.contains("fundacao_indice") ||
                        !mensagem.contains("destino_indice")) {
                        throw std::invalid_argument("Dados da fundação incompletos");
                    }

                    int fundacao = mensagem["fundacao_indice"];
                    int destino = mensagem["destino_indice"];

                    if (fundacao < 0 || destino < 0)
                        throw std::out_of_range("Índice da fundação inválido");

                    jogo.moverDaFundacao(
                        fundacao,
                        TipoPilha::Coluna,
                        destino
                    );
                }
                else if (acao == "MOVER_UMA_PARA_FUNDACAO") {
                    bool moveu = jogo.moverUmaParaFundacao();

                    if (jogo.getVitoria())
                        jogo.salvarRecord();

                    json resposta = estadoParaJson(jogo);
                    resposta["movimento_realizado"] = moveu;

                    respostaTexto = resposta.dump();
                    destinatarios = conexoes;
                }
                else {
                    throw std::invalid_argument("Ação da Paciência não reconhecida");
                }

                if (destinatarios.empty()) {
                    if (jogo.getVitoria())
                        jogo.salvarRecord();

                    respostaTexto = estadoParaJson(jogo).dump();
                    destinatarios = conexoes;
                }
            }

            for (crow::websocket::connection* destino : destinatarios) {
                if (destino)
                    destino->send_text(respostaTexto);
            }
        }
        catch (const json::exception& erro) {
            enviarErro(conn, "Mensagem JSON inválida: " + std::string(erro.what()));
        }
        catch (const std::exception& erro) {
            std::cerr << "Erro no processamento da Paciência: "
                      << erro.what() << '\n';

            enviarErro(conn, erro.what());
        }
    });
}