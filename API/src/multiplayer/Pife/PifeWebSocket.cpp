#include "multiplayer/Pife/PifeWebSocket.hpp"
#include "multiplayer/Pife/GerenciadorSalasPife.hpp"
#include "ExcecoesPife.hpp"

#include <exception>
#include <string>

std::map<crow::websocket::connection*, SessaoWebSocket>
    PifeWebSocket::sessoes_;

void PifeWebSocket::registrar(crow::SimpleApp& app) {
    CROW_WEBSOCKET_ROUTE(app, "/ws/pife")
    .onopen([](crow::websocket::connection& conn) {
        crow::json::wvalue resposta;
        resposta["tipo"] = "conectado";
        resposta["mensagem"] =
            "Conectado ao servidor do Pife";

        enviarMensagem(conn, resposta);
    })
    .onmessage([](
        crow::websocket::connection& conn,
        const std::string& data,
        bool isBinary
    ) {
        if (isBinary) {
            enviarErro(
                conn,
                "Mensagens binárias não são aceitas"
            );
            return;
        }

        auto dados = crow::json::load(data);

        if (!dados) {
            enviarErro(conn, "Mensagem JSON inválida");
            return;
        }

        if (!dados.has("tipo")) {
            enviarErro(
                conn,
                "A mensagem não possui o campo tipo"
            );
            return;
        }

        std::string tipo = dados["tipo"].s();

        if (tipo == "entrar_sala") {
            entrarNaSala(conn, dados);
        }
        else if (tipo == "acao_jogo") {
            processarAcao(conn, dados);
        }
        else if (tipo == "ping") {
            processarPing(conn);
        }
        else if (tipo == "obter_estado") {
            auto sessaoIt = sessoes_.find(&conn);

            if (sessaoIt == sessoes_.end()) {
                enviarErro(
                    conn,
                    "Você ainda não entrou em uma sala"
                );
                return;
            }

            SalaPife* sala =
                GerenciadorSalasPife::obterSala(
                    sessaoIt->second.idSala
                );

            if (!sala) {
                enviarErro(conn, "Sala não encontrada");
                return;
            }

            try {
                enviarEstadoJogador(
                    sala,
                    sessaoIt->second.idJogador,
                    conn
                );
            }
            catch (const ErroPife& erro) {
                enviarErro(conn, erro.what());
            }
            catch (const std::exception& erro) {
                enviarErro(
                    conn,
                    std::string(
                        "Erro interno ao obter o estado: "
                    ) + erro.what()
                );
            }
        }
        else {
            enviarErro(
                conn,
                "Tipo de mensagem não reconhecido"
            );
        }
    })
    .onclose([](
        crow::websocket::connection& conn,
        const std::string&,
        uint16_t
    ) {
        removerConexao(conn);
    });
}

void PifeWebSocket::entrarNaSala(
    crow::websocket::connection& conn,
    const crow::json::rvalue& dados
) {
    if (sessoes_.find(&conn) != sessoes_.end()) {
        enviarErro(
            conn,
            "Esta conexão já entrou em uma sala"
        );
        return;
    }

    if (!dados.has("sala")) {
        enviarErro(conn, "Informe o código da sala");
        return;
    }

    if (!dados.has("token")) {
        enviarErro(
            conn,
            "Informe o token de reconexão"
        );
        return;
    }

    std::string idSala = dados["sala"].s();
    std::string tokenReconexao = dados["token"].s();
    std::string nome = dados.has("nome")
        ? std::string(dados["nome"].s())
        : "";

    if (idSala.empty()) {
        enviarErro(
            conn,
            "O código da sala é inválido"
        );
        return;
    }

    if (!tokenValido(tokenReconexao)) {
        enviarErro(
            conn,
            "O token de reconexão é inválido"
        );
        return;
    }

    if (nome.size() > 20) {
        nome.resize(20);
    }

    SalaPife* sala =
        GerenciadorSalasPife::obterSala(idSala);

    if (!sala) {
        enviarErro(conn, "Sala não encontrada");
        return;
    }

    bool reconexao =
        sala->podeReconectar(tokenReconexao);

    if (sala->partidaIniciada() && !reconexao) {
        enviarErro(
            conn,
            "A partida desta sala já foi iniciada"
        );
        return;
    }

    if (
        !reconexao &&
        !sala->podeReceberNovoJogador()
    ) {
        enviarErro(conn, "A sala está cheia");
        return;
    }

    crow::websocket::connection*
        conexaoAnterior = nullptr;

    if (reconexao) {
        int idAnterior =
            sala->buscarJogadorPorToken(
                tokenReconexao
            );

        conexaoAnterior =
            sala->obterConexaoJogador(idAnterior);
    }

    try {
        int idJogador = sala->adicionarJogador(
            &conn,
            tokenReconexao,
            nome
        );

        if (idJogador == -1) {
            enviarErro(
                conn,
                reconexao
                    ? "Não foi possível reconectar o jogador"
                    : "Não foi possível entrar na sala"
            );
            return;
        }

        if (
            conexaoAnterior &&
            conexaoAnterior != &conn
        ) {
            sessoes_.erase(conexaoAnterior);
        }

        sessoes_[&conn] = {
            idSala,
            idJogador,
            tokenReconexao
        };

        const JogadorPife& jogador =
            sala->jogo().consultarJogador(
                idJogador
            );

        crow::json::wvalue resposta;

        resposta["tipo"] = reconexao
            ? "reconexao_confirmada"
            : "entrada_confirmada";

        resposta["sala"] = idSala;
        resposta["idJogador"] = idJogador;
        resposta["nome"] = jogador.nome();
        resposta["maxJogadores"] =
            sala->maxJogadores();

        resposta["jogadoresConectados"] =
            sala->jogadoresConectados();

        resposta["jogadoresRegistrados"] =
            sala->jogadoresRegistrados();

        resposta["partidaIniciada"] =
            sala->partidaIniciada();

        resposta["reconectado"] = reconexao;

        enviarMensagem(conn, resposta);
        enviarEstadoSala(sala);
    }
    catch (const ErroPife& erro) {
        enviarErro(conn, erro.what());
    }
    catch (const std::exception& erro) {
        enviarErro(
            conn,
            std::string(
                "Erro interno ao entrar na sala: "
            ) + erro.what()
        );
    }
}

void PifeWebSocket::processarAcao(
    crow::websocket::connection& conn,
    const crow::json::rvalue& dados
) {
    auto sessaoIt = sessoes_.find(&conn);

    if (sessaoIt == sessoes_.end()) {
        enviarErro(
            conn,
            "Você ainda não entrou em uma sala"
        );
        return;
    }

    if (!dados.has("acao")) {
        enviarErro(
            conn,
            "A mensagem não possui o campo acao"
        );
        return;
    }

    const SessaoWebSocket& sessao =
        sessaoIt->second;

    SalaPife* sala =
        GerenciadorSalasPife::obterSala(
            sessao.idSala
        );

    if (!sala) {
        enviarErro(conn, "Sala não encontrada");
        return;
    }

    if (!sala->partidaIniciada()) {
        enviarErro(
            conn,
            "Aguardando os outros jogadores entrarem"
        );
        return;
    }

    Pife& jogo = sala->jogo();
    int idJogador = sessao.idJogador;
    std::string acao = dados["acao"].s();

    try {
        if (acao == "COMPRAR_BARALHO") {
            jogo.comprarBaralho(idJogador);
        }
        else if (acao == "COMPRAR_MESA") {
            jogo.comprarMesa(idJogador);
        }
        else if (acao == "DESCARTAR") {
            if (!dados.has("indice")) {
                enviarErro(
                    conn,
                    "Informe o índice da carta"
                );
                return;
            }

            jogo.colocarNaMesa(
                idJogador,
                dados["indice"].i()
            );
        }
        else if (acao == "ORGANIZAR") {
            jogo.organizarMao(idJogador);
        }
        else if (acao == "BATER") {
            jogo.bati(idJogador);
        }
        else {
            enviarErro(
                conn,
                "Ação do Pife não reconhecida"
            );
            return;
        }

        enviarEstadoSala(sala);
    }
    catch (const ErroPife& erro) {
        enviarErro(conn, erro.what());

        try {
            enviarEstadoJogador(
                sala,
                idJogador,
                conn
            );
        }
        catch (const std::exception&) {
        }
    }
    catch (const std::exception& erro) {
        enviarErro(
            conn,
            std::string(
                "Erro interno ao processar a ação: "
            ) + erro.what()
        );

        try {
            enviarEstadoJogador(
                sala,
                idJogador,
                conn
            );
        }
        catch (const std::exception&) {
        }
    }
}

void PifeWebSocket::processarPing(
    crow::websocket::connection& conn
) {
    crow::json::wvalue resposta;
    resposta["tipo"] = "pong";

    enviarMensagem(conn, resposta);
}

void PifeWebSocket::enviarMensagem(
    crow::websocket::connection& conn,
    const crow::json::wvalue& mensagem
) {
    conn.send_text(mensagem.dump());
}

void PifeWebSocket::enviarErro(
    crow::websocket::connection& conn,
    const std::string& mensagem
) {
    crow::json::wvalue erro;

    erro["tipo"] = "erro";
    erro["erro"] = mensagem;
    erro["mensagem"] = mensagem;

    enviarMensagem(conn, erro);
}

bool PifeWebSocket::tokenValido(
    const std::string& tokenReconexao
) {
    return tokenReconexao.size() >= 8 &&
           tokenReconexao.size() <= 100;
}

void PifeWebSocket::adicionarCartaAoJson(
    crow::json::wvalue& destino,
    const Carta& carta
) {
    destino["valor"] =
        static_cast<int>(carta.mostraValor());

    destino["naipe"] =
        static_cast<int>(carta.mostraNaipe());
}

void PifeWebSocket::enviarEstadoJogador(
    SalaPife* sala,
    int idJogador,
    crow::websocket::connection& conn
) {
    if (!sala) {
        return;
    }

    Pife& jogo = sala->jogo();
    crow::json::wvalue estado;

    estado["tipo"] = "estado_jogo";
    estado["sala"] = sala->idSala();
    estado["meu_id"] = idJogador;
    estado["max_jogadores"] =
        sala->maxJogadores();

    estado["jogadores_conectados"] =
        sala->jogadoresConectados();

    estado["jogadores_registrados"] =
        sala->jogadoresRegistrados();

    estado["partida_iniciada"] =
        sala->partidaIniciada();

    estado["jogador_atual"] =
        jogo.consultarIndiceJogadorAtual();

    estado["jogo_finalizado"] =
        jogo.jogoFinalizado();

    estado["vencedor"] =
        jogo.consultarVencedor();

    estado["quantidade_baralho"] =
        jogo.quantidadeCartasBaralho();

    const JogadorPife& jogadorLocal =
        jogo.consultarJogador(idJogador);

    estado["meu_nome"] =
        jogadorLocal.nome().empty()
            ? "Jogador " +
                std::to_string(idJogador + 1)
            : jogadorLocal.nome();

    FaseTurno fase =
        jogo.consultarFaseTurno();

    if (
        fase ==
        FaseTurno::AguardandoCompra
    ) {
        estado["fase"] =
            "AGUARDANDO_COMPRA";
    }
    else if (
        fase ==
        FaseTurno::AguardandoDescarte
    ) {
        estado["fase"] =
            "AGUARDANDO_DESCARTE";
    }
    else {
        estado["fase"] = "FINALIZADO";
    }

    bool partidaDisponivel =
        sala->partidaIniciada() &&
        !jogo.jogoFinalizado();

    estado["pode_comprar_baralho"] =
        partidaDisponivel &&
        jogo.podeComprarBaralho(
            idJogador
        );

    estado["pode_comprar_mesa"] =
        partidaDisponivel &&
        jogo.podeComprarMesa(
            idJogador
        );

    estado["pode_descartar"] =
        partidaDisponivel &&
        jogo.podeColocarNaMesa(
            idJogador
        );

    estado["pode_bater"] =
        partidaDisponivel &&
        jogo.podeBater(idJogador);

    const std::vector<Carta>& mao =
        jogo.consultarMao(idJogador);

    estado["minha_mao"] =
        crow::json::wvalue::list();

    for (
        std::size_t i = 0;
        i < mao.size();
        i++
    ) {
        adicionarCartaAoJson(
            estado["minha_mao"][i],
            mao[i]
        );
    }

    const std::vector<Carta>& mesa =
        jogo.consultarMesa();

    estado["mesa"] =
        crow::json::wvalue::list();

    for (
        std::size_t i = 0;
        i < mesa.size();
        i++
    ) {
        adicionarCartaAoJson(
            estado["mesa"][i],
            mesa[i]
        );
    }

    adicionarCartaAoJson(
        estado["vira"],
        jogo.consultarVira()
    );

    estado["jogadores"] =
        crow::json::wvalue::list();

    for (
        int i = 0;
        i < jogo.numeroDeJogadores();
        i++
    ) {
        const JogadorPife& jogador =
            jogo.consultarJogador(i);

        estado["jogadores"][i]["id"] = i;

        estado["jogadores"][i]["nome"] =
            jogador.nome().empty()
                ? "Jogador " +
                    std::to_string(i + 1)
                : jogador.nome();

        estado["jogadores"][i]
              ["quantidade_cartas"] =
            jogador.tmnhMao();

        estado["jogadores"][i]["sou_eu"] =
            i == idJogador;

        estado["jogadores"][i]["conectado"] =
            sala->jogadorEstaConectado(i);
    }

    enviarMensagem(conn, estado);
}

void PifeWebSocket::enviarEstadoSala(
    SalaPife* sala
) {
    if (!sala) {
        return;
    }

    for (
        const ConexaoPife& registro :
        sala->conexoes()
    ) {
        if (!registro.conexao) {
            continue;
        }

        try {
            enviarEstadoJogador(
                sala,
                registro.idJogador,
                *registro.conexao
            );
        }
        catch (const std::exception& erro) {
            enviarErro(
                *registro.conexao,
                std::string(
                    "Erro ao atualizar o estado: "
                ) + erro.what()
            );
        }
    }
}

void PifeWebSocket::removerConexao(
    crow::websocket::connection& conn
) {
    auto sessaoIt = sessoes_.find(&conn);

    if (sessaoIt == sessoes_.end()) {
        return;
    }

    std::string idSala =
        sessaoIt->second.idSala;

    sessoes_.erase(sessaoIt);

    SalaPife* sala =
        GerenciadorSalasPife::obterSala(
            idSala
        );

    if (!sala) {
        return;
    }

    sala->removerConexao(&conn);
    enviarEstadoSala(sala);
}