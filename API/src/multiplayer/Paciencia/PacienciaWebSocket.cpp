#include "multiplayer/Paciencia/PacienciaWebSocket.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "coreAPI/JsonConversor.hpp"

using json = nlohmann::json;

std::map<crow::websocket::connection*, std::unique_ptr<Paciencia>> PacienciaWebSocket::partidas_;
std::mutex PacienciaWebSocket::mutex_;

json PacienciaWebSocket::estadoParaJson(const Paciencia& jogo) {
    json estado;

    estado["tipo"] = "estado_jogo";
    estado["pontuacao"] = jogo.getPontuacao();
    estado["recorde"] = jogo.getRecord();
    estado["cava_tamanho"] = jogo.getCavaTamanho();
    estado["vitoria"] = jogo.getVitoria();

    estado["cartas_escondidas"] = json::array();

    for (int i = 0; i < 7; ++i)
        estado["cartas_escondidas"].push_back(jogo.getCartasEscondidas(i));

    estado["descarte"] = json::array();

    for (const Carta& carta : jogo.getDescarte())
        estado["descarte"].push_back(JsonConversor::cartaParaJson(carta));

    estado["fundacoes"] = json::array();

    for (const std::vector<Carta>& fundacao : jogo.getFundacoes()) {
        json pilha = json::array();

        for (const Carta& carta : fundacao)
            pilha.push_back(JsonConversor::cartaParaJson(carta));

        estado["fundacoes"].push_back(pilha);
    }

    estado["colunas"] = json::array();

    for (const std::vector<Carta>& coluna : jogo.getColunas()) {
        json pilha = json::array();

        for (const Carta& carta : coluna)
            pilha.push_back(JsonConversor::cartaParaJson(carta));

        estado["colunas"].push_back(pilha);
    }

    return estado;
}

TipoPilha PacienciaWebSocket::stringParaTipoPilha(const std::string& tipo) {
    if (tipo == "coluna")
        return TipoPilha::Coluna;

    if (tipo == "descarte")
        return TipoPilha::Descarte;

    if (tipo == "fundacao")
        return TipoPilha::Fundacao;

    throw std::invalid_argument("Tipo de pilha inválido");
}

void PacienciaWebSocket::processarAcao(crow::websocket::connection& conn, const json& dados) {
    auto partidaIt = partidas_.find(&conn);

    if (partidaIt == partidas_.end() || !partidaIt->second)
        return enviarErro(conn, "Partida de Paciência não encontrada");

    if (!dados.contains("acao") || !dados["acao"].is_string())
        return enviarErro(conn, "A mensagem não possui uma ação válida");

    Paciencia& jogo = *partidaIt->second;
    const std::string acao = dados["acao"].get<std::string>();

    if (acao == "OBTER_ESTADO_ATUAL") {
        enviarEstado(conn, jogo);
        return;
    }

    if (acao == "PING") {
        json resposta;
        resposta["tipo"] = "pong";
        conn.send_text(resposta.dump());
        return;
    }

    if (acao == "COMPRAR_CARTA") {
        jogo.comprarCarta();
    } else if (acao == "DESFAZER") {
        jogo.desfazer();
    } else if (acao == "NOVO_JOGO") {
        jogo.gerarJogoReversivel();
    } else if (acao == "COMPLETAR_AUTOMATICAMENTE") {
        jogo.completarAutomaticamente();
    } else if (acao == "MOVER") {
        if (!dados.contains("origem_tipo") || !dados.contains("origem_indice") ||
            !dados.contains("destino_tipo") || !dados.contains("destino_indice")) {
            return enviarErro(conn, "Parâmetros incompletos para mover a carta");
        }

        jogo.mover(
            stringParaTipoPilha(dados["origem_tipo"].get<std::string>()),
            dados["origem_indice"].get<int>(),
            stringParaTipoPilha(dados["destino_tipo"].get<std::string>()),
            dados["destino_indice"].get<int>()
        );
    } else if (acao == "MOVER_BLOCO") {
        if (!dados.contains("origem_coluna") || !dados.contains("carta_idx") ||
            !dados.contains("destino_coluna")) {
            return enviarErro(conn, "Parâmetros incompletos para mover o bloco");
        }

        jogo.moverBloco(
            dados["origem_coluna"].get<int>(),
            dados["carta_idx"].get<int>(),
            dados["destino_coluna"].get<int>()
        );
    } else if (acao == "MOVER_DA_FUNDACAO") {
        if (!dados.contains("fundacao_indice") || !dados.contains("destino_indice"))
            return enviarErro(conn, "Parâmetros incompletos para mover da fundação");

        jogo.moverDaFundacao(
            dados["fundacao_indice"].get<int>(),
            TipoPilha::Coluna,
            dados["destino_indice"].get<int>()
        );
    } else if (acao == "MOVER_UMA_PARA_FUNDACAO") {
        const bool movimentoRealizado = jogo.moverUmaParaFundacao();

        if (jogo.getVitoria())
            jogo.salvarRecord();

        json resposta = estadoParaJson(jogo);
        resposta["movimento_realizado"] = movimentoRealizado;

        conn.send_text(resposta.dump());
        return;
    } else {
        return enviarErro(conn, "Ação da Paciência não reconhecida");
    }

    if (jogo.getVitoria())
        jogo.salvarRecord();

    enviarEstado(conn, jogo);
}

void PacienciaWebSocket::enviarEstado(crow::websocket::connection& conn, const Paciencia& jogo) {
    conn.send_text(estadoParaJson(jogo).dump());
}

void PacienciaWebSocket::enviarErro(crow::websocket::connection& conn, const std::string& mensagem) {
    json erro;
    erro["tipo"] = "erro";
    erro["erro"] = mensagem;
    erro["mensagem"] = mensagem;

    conn.send_text(erro.dump());
}

void PacienciaWebSocket::registrar(crow::SimpleApp& app) {
    CROW_WEBSOCKET_ROUTE(app, "/ws/paciencia")
    .onopen([](crow::websocket::connection& conn) {
        try {
            auto jogo = std::make_unique<Paciencia>();
            jogo->gerarJogoReversivel();

            std::lock_guard<std::mutex> lock(mutex_);

            partidas_[&conn] = std::move(jogo);
            enviarEstado(conn, *partidas_.at(&conn));
        } catch (const std::exception& erro) {
            std::cerr << "Erro ao iniciar Paciência: " << erro.what() << '\n';
            enviarErro(conn, "Não foi possível iniciar a partida");
        }
    })
    .onmessage([](crow::websocket::connection& conn, const std::string& mensagem, bool) {
        try {
            const json dados = json::parse(mensagem);

            std::lock_guard<std::mutex> lock(mutex_);
            processarAcao(conn, dados);
        } catch (const json::exception& erro) {
            std::cerr << "JSON inválido na Paciência: " << erro.what() << '\n';
            enviarErro(conn, "Mensagem JSON inválida");
        } catch (const std::exception& erro) {
            std::cerr << "Erro ao processar Paciência: " << erro.what() << '\n';
            enviarErro(conn, erro.what());
        }
    })
    .onclose([](crow::websocket::connection& conn, const std::string&, uint16_t) {
        std::lock_guard<std::mutex> lock(mutex_);
        partidas_.erase(&conn);
    });
}