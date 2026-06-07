#include "multiplayer/Pife/PifeWebSocket.hpp"

std::map<std::string, std::unique_ptr<SalaPife>>
PifeWebSocket::salas_;

std::map<crow::websocket::connection*, SessaoPife>
PifeWebSocket::sessoes_;

void PifeWebSocket::registrar(crow::SimpleApp& app) {

    CROW_WEBSOCKET_ROUTE(app, "/ws/pife")

    .onopen([](crow::websocket::connection& conn) {

        crow::json::wvalue resposta;

        resposta["tipo"] = "conectado";
        resposta["mensagem"] = "Conectado ao WebSocket do Pife";

        enviarMensagem(conn, resposta);

    })

    .onmessage([](
        crow::websocket::connection& conn,
        const std::string& data,
        bool is_binary
    ) {

        if (is_binary) {
            return;
        }

        auto dados = crow::json::load(data);

        if (!dados) {
            crow::json::wvalue erro;

            erro["tipo"] = "erro";
            erro["mensagem"] = "Mensagem JSON inválida";

            enviarMensagem(conn, erro);
            return;
        }

        std::string tipo = dados["tipo"].s();

        if (tipo == "entrar_sala") {
            entrarNaSala(conn, dados);
            return;
        }

        crow::json::wvalue erro;

        erro["tipo"] = "erro";
        erro["mensagem"] = "Tipo de mensagem não reconhecido";

        enviarMensagem(conn, erro);

    })

    .onclose([](
        crow::websocket::connection& conn,
        const std::string& reason,
        uint16_t code
    ) {

        removerConexao(conn);

    });
}

SalaPife* PifeWebSocket::obterOuCriarSala(
    const std::string& idSala,
    int maxJogadores
) {

    auto it = salas_.find(idSala);

    if (it != salas_.end()) {
        return it->second.get();
    }

    salas_[idSala] = std::make_unique<SalaPife>(
        idSala,
        maxJogadores
    );

    return salas_[idSala].get();
}

void PifeWebSocket::entrarNaSala(
    crow::websocket::connection& conn,
    const crow::json::rvalue& dados
) {

    std::string idSala = "sala_teste";
    int maxJogadores = 2;

    if (dados.has("sala")) {
        idSala = dados["sala"].s();
    }

    if (dados.has("jogadores")) {
        maxJogadores = dados["jogadores"].i();
    }

    SalaPife* sala = obterOuCriarSala(
        idSala,
        maxJogadores
    );

    int idJogador = sala->adicionarJogador(&conn);

    if (idJogador == -1) {
        crow::json::wvalue erro;

        erro["tipo"] = "erro";
        erro["mensagem"] = "Sala cheia";

        enviarMensagem(conn, erro);
        return;
    }

    sessoes_[&conn] = {
        idSala,
        idJogador
    };

    crow::json::wvalue resposta;

    resposta["tipo"] = "entrada_confirmada";
    resposta["sala"] = idSala;
    resposta["idJogador"] = idJogador;
    resposta["maxJogadores"] = sala->maxJogadores();
    resposta["jogadoresConectados"] = sala->jogadoresConectados();

    enviarMensagem(conn, resposta);

    enviarEstadoSala(sala);
}

void PifeWebSocket::enviarMensagem(
    crow::websocket::connection& conn,
    const crow::json::wvalue& mensagem
) {

    conn.send_text(mensagem.dump());
}

void PifeWebSocket::enviarEstadoSala(SalaPife* sala) {

    crow::json::wvalue estado;

    estado["tipo"] = "estado_sala";
    estado["sala"] = sala->idSala();
    estado["maxJogadores"] = sala->maxJogadores();
    estado["jogadoresConectados"] = sala->jogadoresConectados();

    for (auto& conexao : sala->conexoes()) {
        estado["jogadores"][conexao.idJogador]["idJogador"] =
            conexao.idJogador;

        estado["jogadores"][conexao.idJogador]["conectado"] =
            true;
    }

    for (auto& conexao : sala->conexoes()) {
        if (conexao.conexao != nullptr) {
            conexao.conexao->send_text(estado.dump());
        }
    }
}

void PifeWebSocket::removerConexao(
    crow::websocket::connection& conn
) {

    auto it = sessoes_.find(&conn);

    if (it == sessoes_.end()) {
        return;
    }

    std::string idSala = it->second.idSala;

    sessoes_.erase(it);

    auto salaIt = salas_.find(idSala);

    if (salaIt == salas_.end()) {
        return;
    }

    SalaPife* sala = salaIt->second.get();

    for (auto& conexao : sala->conexoes()) {
        if (conexao.conexao == &conn) {
            conexao.conexao = nullptr;
        }
    }

    enviarEstadoSala(sala);
}