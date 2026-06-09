#include "multiplayer/Poker/PokerWebSocket.hpp"
#include "multiplayer/Poker/GerenciadorSalasPoker.hpp"
#include <algorithm>

std::map<crow::websocket::connection*, SessaoWebSocket> PokerWebSocket::sessoes_;

void PokerWebSocket::registrar(crow::SimpleApp& app) {
    CROW_WEBSOCKET_ROUTE(app, "/ws/poker")
    .onopen([](crow::websocket::connection& conn) {
        enviarMensagem(conn, {{"tipo", "conectado"}, {"mensagem", "Conectado ao servidor do Poker"}});
    })
    .onmessage([](crow::websocket::connection& conn, const std::string& data, bool binario) {
        if (binario) return enviarErro(conn, "Mensagens binárias não são aceitas");

        auto dados = crow::json::load(data);
        if (!dados) return enviarErro(conn, "Mensagem JSON inválida");
        if (!dados.has("tipo")) return enviarErro(conn, "A mensagem não possui o campo tipo");

        std::string tipo = dados["tipo"].s();

        if (tipo == "entrar_sala") entrarNaSala(conn, dados);
        else if (tipo == "acao_jogo") processarAcao(conn, dados);
        else if (tipo == "ping") processarPing(conn);
        else if (tipo == "obter_estado") {
            auto it = sessoes_.find(&conn);
            if (it == sessoes_.end()) return enviarErro(conn, "Você ainda não entrou em uma sala");

            SalaPoker* sala = GerenciadorSalasPoker::obterSala(it->second.idSala);
            if (!sala) return enviarErro(conn, "Sala não encontrada");

            enviarEstadoJogador(sala, it->second.idJogador, conn);
        } else enviarErro(conn, "Tipo de mensagem não reconhecido");
    })
    .onclose([](crow::websocket::connection& conn, const std::string&, uint16_t) {
        removerConexao(conn);
    });
}

void PokerWebSocket::entrarNaSala(crow::websocket::connection& conn, const crow::json::rvalue& dados) {
    if (sessoes_.find(&conn) != sessoes_.end()) return enviarErro(conn, "Esta conexão já entrou em uma sala");
    if (!dados.has("sala")) return enviarErro(conn, "Informe o código da sala");
    if (!dados.has("token")) return enviarErro(conn, "Informe o token de reconexão");

    std::string idSala = dados["sala"].s();
    std::string token = dados["token"].s();
    std::string nome = dados.has("nome") ? std::string(dados["nome"].s()) : "";

    if (idSala.empty()) return enviarErro(conn, "O código da sala é inválido");
    if (!tokenValido(token)) return enviarErro(conn, "O token de reconexão é inválido");

    SalaPoker* sala = GerenciadorSalasPoker::obterSala(idSala);
    if (!sala) return enviarErro(conn, "Sala não encontrada");

    bool reconexao = sala->podeReconectar(token);
    if (sala->partidaIniciada() && !reconexao) return enviarErro(conn, "A partida desta sala já foi iniciada");
    if (!reconexao && !sala->podeReceberNovoJogador()) return enviarErro(conn, "A sala está cheia");

    crow::websocket::connection* conexaoAnterior = nullptr;

    if (reconexao) {
        int idAnterior = sala->buscarJogadorPorToken(token);
        conexaoAnterior = sala->obterConexaoJogador(idAnterior);
    }

    int idJogador = sala->adicionarJogador(&conn, token, nome);
    if (idJogador == -1) return enviarErro(conn, reconexao ? "Não foi possível reconectar o jogador" : "Não foi possível entrar na sala");

    if (conexaoAnterior && conexaoAnterior != &conn) sessoes_.erase(conexaoAnterior);
    sessoes_[&conn] = {idSala, idJogador, token};

    enviarMensagem(conn, {
        {"tipo", reconexao ? "reconexao_confirmada" : "entrada_confirmada"},
        {"sala", idSala},
        {"idJogador", idJogador},
        {"maxJogadores", sala->maxJogadores()},
        {"jogadoresConectados", sala->jogadoresConectados()},
        {"jogadoresRegistrados", sala->jogadoresRegistrados()},
        {"partidaIniciada", sala->partidaIniciada()},
        {"reconectado", reconexao}
    });

    enviarEstadoSala(sala);
}

void PokerWebSocket::processarAcao(crow::websocket::connection& conn, const crow::json::rvalue& dados) {
    auto it = sessoes_.find(&conn);
    if (it == sessoes_.end()) return enviarErro(conn, "Você ainda não entrou em uma sala");
    if (!dados.has("acao")) return enviarErro(conn, "A mensagem não possui o campo acao");

    SalaPoker* sala = GerenciadorSalasPoker::obterSala(it->second.idSala);
    if (!sala) return enviarErro(conn, "Sala não encontrada");
    if (!sala->partidaIniciada()) return enviarErro(conn, "Aguardando os outros jogadores entrarem");

    std::string acao = dados["acao"].s();
    int idJogador = it->second.idJogador;

    if (acao == "CONFIRMAR_TROCA") confirmarTroca(conn, *sala, idJogador, dados);
    else if (acao == "NOVA_RODADA") {
        if (idJogador != 0) return enviarErro(conn, "Somente o primeiro jogador pode iniciar uma nova rodada");
        iniciarNovaRodada(conn, *sala);
    } else enviarErro(conn, "Ação do Poker não reconhecida");
}

void PokerWebSocket::confirmarTroca(crow::websocket::connection& conn, SalaPoker& sala, int idJogador, const crow::json::rvalue& dados) {
    std::vector<int> indices;

    if (!extrairIndicesTroca(dados, indices)) {
        enviarErro(conn, "Os índices da troca são inválidos");
        return;
    }

    if (!sala.confirmarTroca(idJogador, indices)) {
        enviarErro(conn, "Não é possível confirmar esta troca agora");
        enviarEstadoJogador(&sala, idJogador, conn);
        return;
    }

    enviarEstadoSala(&sala);
}

void PokerWebSocket::iniciarNovaRodada(crow::websocket::connection& conn, SalaPoker& sala) {
    if (!sala.iniciarNovaRodada()) {
        enviarErro(conn, "Não é possível iniciar uma nova rodada agora");
        return;
    }

    enviarEstadoSala(&sala);
}

bool PokerWebSocket::extrairIndicesTroca(const crow::json::rvalue& dados, std::vector<int>& indices) {
    indices.clear();
    if (!dados.has("indices") || dados["indices"].t() != crow::json::type::List) return false;

    for (const auto& item : dados["indices"]) {
        if (item.t() != crow::json::type::Number) return false;
        int indice = item.i();
        if (indice < 0 || indice >= 5 || std::find(indices.begin(), indices.end(), indice) != indices.end()) return false;
        indices.push_back(indice);
    }

    return indices.size() <= 3;
}

void PokerWebSocket::processarPing(crow::websocket::connection& conn) {
    enviarMensagem(conn, {{"tipo", "pong"}});
}

void PokerWebSocket::enviarMensagem(crow::websocket::connection& conn, const json& mensagem) {
    conn.send_text(mensagem.dump());
}

void PokerWebSocket::enviarErro(crow::websocket::connection& conn, const std::string& mensagem) {
    enviarMensagem(conn, {{"tipo", "erro"}, {"erro", mensagem}, {"mensagem", mensagem}});
}

void PokerWebSocket::enviarEstadoJogador(SalaPoker* sala, int idJogador, crow::websocket::connection& conn) {
    if (sala) enviarMensagem(conn, sala->gerarJson(idJogador));
}

void PokerWebSocket::enviarEstadoSala(SalaPoker* sala) {
    if (!sala) return;
    for (const auto& registro : sala->conexoes())
        if (registro.conexao) enviarEstadoJogador(sala, registro.idJogador, *registro.conexao);
}

void PokerWebSocket::removerConexao(crow::websocket::connection& conn) {
    auto it = sessoes_.find(&conn);
    if (it == sessoes_.end()) return;

    std::string idSala = it->second.idSala;
    sessoes_.erase(it);

    SalaPoker* sala = GerenciadorSalasPoker::obterSala(idSala);
    if (!sala) return;

    sala->removerConexao(&conn);
    enviarEstadoSala(sala);
    GerenciadorSalasPoker::removerSalaSeVazia(idSala);
}

bool PokerWebSocket::tokenValido(const std::string& tokenReconexao) {
    return tokenReconexao.size() >= 8 && tokenReconexao.size() <= 100;
}