#include "multiplayer/Poker/PokerWebSocket.hpp"

#include <algorithm>

std::map<crow::websocket::connection*, SessaoWebSocket> PokerWebSocket::sessoes_;

void PokerWebSocket::registrar(crow::SimpleApp& app) {
    CROW_WEBSOCKET_ROUTE(app, "/ws/poker")
    .onopen([](crow::websocket::connection& conexao) {
        enviarMensagem(conexao, {
            {"tipo", "conectado"},
            {"mensagem", "Conectado ao servidor do Poker"}
        });
    })
    .onmessage([](crow::websocket::connection& conexao,
                  const std::string& dadosTexto,
                  bool binario) {
        if (binario) {
            enviarErro(conexao, "Mensagens binárias não são aceitas");
            return;
        }

        auto dados = crow::json::load(dadosTexto);

        if (!dados) {
            enviarErro(conexao, "Mensagem JSON inválida");
            return;
        }

        if (!dados.has("tipo")) {
            enviarErro(conexao, "A mensagem não possui o campo tipo");
            return;
        }

        std::string tipo = dados["tipo"].s();

        if (tipo == "entrar_sala") {
            entrarNaSala(conexao, dados);
        } else if (tipo == "acao_jogo") {
            processarAcao(conexao, dados);
        } else if (tipo == "ping") {
            processarPing(conexao);
        } else if (tipo == "obter_estado") {
            auto it = sessoes_.find(&conexao);

            if (it == sessoes_.end()) {
                enviarErro(conexao, "Você ainda não entrou em uma sala");
                return;
            }

            SalaPoker* sala =
                GerenciadorSalasPoker::obterSala(it->second.idSala);

            if (sala == nullptr) {
                enviarErro(conexao, "Sala não encontrada");
                return;
            }

            enviarEstadoJogador(
                sala,
                it->second.idJogador,
                conexao
            );
        } else {
            enviarErro(conexao, "Tipo de mensagem não reconhecido");
        }
    })
    .onclose([](crow::websocket::connection& conexao,
                const std::string& motivo,
                uint16_t codigo) {
        (void)motivo;
        (void)codigo;

        removerConexao(conexao);
    });
}

void PokerWebSocket::entrarNaSala(
    crow::websocket::connection& conexao,
    const crow::json::rvalue& dados
) {
    if (sessoes_.find(&conexao) != sessoes_.end()) {
        enviarErro(conexao, "Esta conexão já entrou em uma sala");
        return;
    }

    if (!dados.has("sala")) {
        enviarErro(conexao, "Informe o código da sala");
        return;
    }

    if (!dados.has("token")) {
        enviarErro(conexao, "Informe o token de reconexão");
        return;
    }

    std::string idSala = dados["sala"].s();
    std::string token = dados["token"].s();
    std::string nome =
        dados.has("nome") ? std::string(dados["nome"].s()) : "";

    if (idSala.empty()) {
        enviarErro(conexao, "O código da sala é inválido");
        return;
    }

    if (!tokenValido(token)) {
        enviarErro(conexao, "O token de reconexão é inválido");
        return;
    }

    SalaPoker* sala = GerenciadorSalasPoker::obterSala(idSala);

    if (sala == nullptr) {
        enviarErro(conexao, "Sala não encontrada");
        return;
    }

    bool reconexao = sala->podeReconectar(token);

    if (sala->partidaIniciada() && !reconexao) {
        enviarErro(
            conexao,
            "A partida desta sala já foi iniciada"
        );
        return;
    }

    if (!reconexao && !sala->podeReceberNovoJogador()) {
        enviarErro(conexao, "A sala está cheia");
        return;
    }

    crow::websocket::connection* conexaoAnterior = nullptr;

    if (reconexao) {
        int idAnterior = sala->buscarJogadorPorToken(token);
        conexaoAnterior = sala->obterConexaoJogador(idAnterior);
    }

    int idJogador =
        sala->adicionarJogador(&conexao, token, nome);

    if (idJogador == -1) {
        enviarErro(
            conexao,
            reconexao
                ? "Não foi possível reconectar o jogador"
                : "Não foi possível entrar na sala"
        );
        return;
    }

    if (conexaoAnterior != nullptr &&
        conexaoAnterior != &conexao) {
        sessoes_.erase(conexaoAnterior);
    }

    sessoes_[&conexao] = {
        idSala,
        idJogador,
        token
    };

    enviarMensagem(conexao, {
        {"tipo",
         reconexao
            ? "reconexao_confirmada"
            : "entrada_confirmada"},
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

void PokerWebSocket::processarAcao(
    crow::websocket::connection& conexao,
    const crow::json::rvalue& dados
) {
    auto it = sessoes_.find(&conexao);

    if (it == sessoes_.end()) {
        enviarErro(conexao, "Você ainda não entrou em uma sala");
        return;
    }

    if (!dados.has("acao")) {
        enviarErro(conexao, "A mensagem não possui o campo acao");
        return;
    }

    SalaPoker* sala =
        GerenciadorSalasPoker::obterSala(it->second.idSala);

    if (sala == nullptr) {
        enviarErro(conexao, "Sala não encontrada");
        return;
    }

    if (!sala->partidaIniciada()) {
        enviarErro(
            conexao,
            "Aguardando os outros jogadores entrarem"
        );
        return;
    }

    std::string acao = dados["acao"].s();
    int idJogador = it->second.idJogador;

    if (acao == "CONFIRMAR_TROCA") {
        confirmarTroca(
            conexao,
            *sala,
            idJogador,
            dados
        );
    } else if (acao == "NOVA_RODADA") {
        if (idJogador != 0) {
            enviarErro(
                conexao,
                "Somente o primeiro jogador pode iniciar uma nova rodada"
            );
            return;
        }

        iniciarNovaRodada(conexao, *sala);
    } else {
        enviarErro(conexao, "Ação do Poker não reconhecida");
    }
}

void PokerWebSocket::confirmarTroca(
    crow::websocket::connection& conexao,
    SalaPoker& sala,
    int idJogador,
    const crow::json::rvalue& dados
) {
    std::vector<int> indices;

    if (!extrairIndicesTroca(dados, indices)) {
        enviarErro(conexao, "Os índices da troca são inválidos");
        return;
    }

    if (!sala.confirmarTroca(idJogador, indices)) {
        enviarErro(
            conexao,
            "Não é possível confirmar esta troca agora"
        );

        enviarEstadoJogador(
            &sala,
            idJogador,
            conexao
        );

        return;
    }

    enviarEstadoSala(&sala);
}

void PokerWebSocket::iniciarNovaRodada(
    crow::websocket::connection& conexao,
    SalaPoker& sala
) {
    if (!sala.iniciarNovaRodada()) {
        enviarErro(
            conexao,
            "Não é possível iniciar uma nova rodada agora"
        );
        return;
    }

    enviarEstadoSala(&sala);
}

bool PokerWebSocket::extrairIndicesTroca(
    const crow::json::rvalue& dados,
    std::vector<int>& indices
) {
    indices.clear();

    if (!dados.has("indices") ||
        dados["indices"].t() != crow::json::type::List) {
        return false;
    }

    for (const auto& item : dados["indices"]) {
        if (item.t() != crow::json::type::Number) {
            return false;
        }

        int indice = item.i();

        if (indice < 0 || indice >= 5) {
            return false;
        }

        if (std::find(
                indices.begin(),
                indices.end(),
                indice
            ) != indices.end()) {
            return false;
        }

        indices.push_back(indice);
    }

    return indices.size() <= 3;
}

bool PokerWebSocket::tokenValido(
    const std::string& tokenReconexao
) {
    return tokenReconexao.size() >= 8 &&
           tokenReconexao.size() <= 100;
}

void PokerWebSocket::processarPing(
    crow::websocket::connection& conexao
) {
    enviarMensagem(conexao, {
        {"tipo", "pong"}
    });
}

void PokerWebSocket::enviarMensagem(
    crow::websocket::connection& conexao,
    const json& mensagem
) {
    conexao.send_text(mensagem.dump());
}

void PokerWebSocket::enviarErro(
    crow::websocket::connection& conexao,
    const std::string& mensagem
) {
    enviarMensagem(conexao, {
        {"tipo", "erro"},
        {"erro", mensagem},
        {"mensagem", mensagem}
    });
}

void PokerWebSocket::enviarEstadoJogador(
    SalaPoker* sala,
    int idJogador,
    crow::websocket::connection& conexao
) {
    if (sala == nullptr) return;

    enviarMensagem(
        conexao,
        sala->gerarJson(idJogador)
    );
}

void PokerWebSocket::enviarEstadoSala(
    SalaPoker* sala
) {
    if (sala == nullptr) return;

    for (const ConexaoPoker& registro : sala->conexoes()) {
        if (registro.conexao != nullptr) {
            enviarEstadoJogador(
                sala,
                registro.idJogador,
                *registro.conexao
            );
        }
    }
}

void PokerWebSocket::removerConexao(
    crow::websocket::connection& conexao
) {
    auto it = sessoes_.find(&conexao);

    if (it == sessoes_.end()) return;

    std::string idSala = it->second.idSala;
    sessoes_.erase(it);

    SalaPoker* sala =
        GerenciadorSalasPoker::obterSala(idSala);

    if (sala == nullptr) return;

    sala->removerConexao(&conexao);
    enviarEstadoSala(sala);
    GerenciadorSalasPoker::removerSalaSeVazia(idSala);
}
