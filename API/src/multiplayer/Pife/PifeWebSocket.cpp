#include "multiplayer/Pife/PifeWebSocket.hpp"
#include "multiplayer/Pife/GerenciadorSalasPife.hpp"

std::map<crow::websocket::connection*, SessaoWebSocket> PifeWebSocket::sessoes_;

void PifeWebSocket::registrar(crow::SimpleApp& app) {
    CROW_WEBSOCKET_ROUTE(app, "/ws/pife")
    .onopen([](crow::websocket::connection& conn) {
        crow::json::wvalue resposta;
        resposta["tipo"] = "conectado";
        resposta["mensagem"] = "Conectado ao servidor do Pife";
        enviarMensagem(conn, resposta);
    })
    .onmessage([](crow::websocket::connection& conn, const std::string& data, bool isBinary) {
        if (isBinary)
            return enviarErro(conn, "Mensagens binárias não são aceitas");

        auto dados = crow::json::load(data);

        if (!dados)
            return enviarErro(conn, "Mensagem JSON inválida");

        if (!dados.has("tipo"))
            return enviarErro(conn, "A mensagem não possui o campo tipo");

        std::string tipo = dados["tipo"].s();

        if (tipo == "entrar_sala") {
            entrarNaSala(conn, dados);
        } else if (tipo == "acao_jogo") {
            processarAcao(conn, dados);
        } else if (tipo == "ping") {
            processarPing(conn);
        } else if (tipo == "obter_estado") {
            auto sessaoIt = sessoes_.find(&conn);

            if (sessaoIt == sessoes_.end())
                return enviarErro(conn, "Você ainda não entrou em uma sala");

            SalaPife* sala = GerenciadorSalasPife::obterSala(sessaoIt->second.idSala);

            if (!sala)
                return enviarErro(conn, "Sala não encontrada");

            enviarEstadoJogador(sala, sessaoIt->second.idJogador, conn);
        } else {
            enviarErro(conn, "Tipo de mensagem não reconhecido");
        }
    })
    .onclose([](crow::websocket::connection& conn, const std::string&, uint16_t) {
        removerConexao(conn);
    });
}

void PifeWebSocket::entrarNaSala(crow::websocket::connection& conn, const crow::json::rvalue& dados) {
    if (sessoes_.find(&conn) != sessoes_.end())
        return enviarErro(conn, "Esta conexão já entrou em uma sala");

    if (!dados.has("sala"))
        return enviarErro(conn, "Informe o código da sala");

    if (!dados.has("token"))
        return enviarErro(conn, "Informe o token de reconexão");

    std::string idSala = dados["sala"].s();
    std::string tokenReconexao = dados["token"].s();
    std::string nome = dados.has("nome") ? std::string(dados["nome"].s()) : "";

    if (idSala.empty())
        return enviarErro(conn, "O código da sala é inválido");

    if (!tokenValido(tokenReconexao))
        return enviarErro(conn, "O token de reconexão é inválido");

    if (nome.size() > 20)
        nome.resize(20);

    SalaPife* sala = GerenciadorSalasPife::obterSala(idSala);

    if (!sala)
        return enviarErro(conn, "Sala não encontrada");

    bool reconexao = sala->podeReconectar(tokenReconexao);

    if (sala->partidaIniciada() && !reconexao)
        return enviarErro(conn, "A partida desta sala já foi iniciada");

    if (!reconexao && !sala->podeReceberNovoJogador())
        return enviarErro(conn, "A sala está cheia");

    crow::websocket::connection* conexaoAnterior = nullptr;

    if (reconexao) {
        int idAnterior = sala->buscarJogadorPorToken(tokenReconexao);

        if (idAnterior != -1)
            conexaoAnterior = sala->obterConexaoJogador(idAnterior);
    }

    int idJogador = sala->adicionarJogador(&conn, tokenReconexao, nome);

    if (idJogador == -1) {
        return enviarErro(
            conn,
            reconexao
                ? "Não foi possível reconectar o jogador"
                : "Não foi possível entrar na sala"
        );
    }

    if (conexaoAnterior && conexaoAnterior != &conn)
        sessoes_.erase(conexaoAnterior);

    sessoes_[&conn] = {idSala, idJogador, tokenReconexao};

    const JogadorPife& jogador = sala->jogo().consultarJogador(idJogador);

    crow::json::wvalue resposta;
    resposta["tipo"] = reconexao ? "reconexao_confirmada" : "entrada_confirmada";
    resposta["sala"] = idSala;
    resposta["idJogador"] = idJogador;
    resposta["nome"] = jogador.nome();
    resposta["maxJogadores"] = sala->maxJogadores();
    resposta["jogadoresConectados"] = sala->jogadoresConectados();
    resposta["jogadoresRegistrados"] = sala->jogadoresRegistrados();
    resposta["partidaIniciada"] = sala->partidaIniciada();
    resposta["reconectado"] = reconexao;

    enviarMensagem(conn, resposta);
    enviarEstadoSala(sala);
}

void PifeWebSocket::processarAcao(crow::websocket::connection& conn, const crow::json::rvalue& dados) {
    auto sessaoIt = sessoes_.find(&conn);

    if (sessaoIt == sessoes_.end())
        return enviarErro(conn, "Você ainda não entrou em uma sala");

    if (!dados.has("acao"))
        return enviarErro(conn, "A mensagem não possui o campo acao");

    const SessaoWebSocket& sessao = sessaoIt->second;
    SalaPife* sala = GerenciadorSalasPife::obterSala(sessao.idSala);

    if (!sala)
        return enviarErro(conn, "Sala não encontrada");

    if (!sala->partidaIniciada())
        return enviarErro(conn, "Aguardando os outros jogadores entrarem");

    Pife& jogo = sala->jogo();

    if (jogo.jogoFinalizado())
        return enviarErro(conn, "A partida já foi finalizada");

    int idJogador = sessao.idJogador;
    std::string acao = dados["acao"].s();
    bool sucesso = false;
    std::string mensagemErro;

    if (acao == "COMPRAR_BARALHO") {
        sucesso = jogo.comprarBaralho(idJogador);
        mensagemErro = "Não é possível comprar do baralho agora";
    } else if (acao == "COMPRAR_MESA") {
        sucesso = jogo.comprarMesa(idJogador);
        mensagemErro = "Não é possível comprar da mesa agora";
    } else if (acao == "DESCARTAR") {
        if (!dados.has("indice"))
            return enviarErro(conn, "Informe o índice da carta");

        int indice = dados["indice"].i();

        if (indice < 0)
            return enviarErro(conn, "O índice da carta é inválido");

        sucesso = jogo.colocarNaMesa(idJogador, indice);
        mensagemErro = "Não é possível descartar esta carta agora";
    } else if (acao == "ORGANIZAR") {
        sucesso = jogo.organizarMao(idJogador);
        mensagemErro = "Não foi possível organizar a mão";
    } else if (acao == "BATER") {
        sucesso = jogo.bati(idJogador);
        mensagemErro = "Sua mão ainda não forma uma combinação vencedora";
    } else {
        return enviarErro(conn, "Ação do Pife não reconhecida");
    }

    if (!sucesso) {
        enviarErro(conn, mensagemErro);
        enviarEstadoJogador(sala, idJogador, conn);
        return;
    }

    enviarEstadoSala(sala);
}

void PifeWebSocket::processarPing(crow::websocket::connection& conn) {
    crow::json::wvalue resposta;
    resposta["tipo"] = "pong";
    enviarMensagem(conn, resposta);
}

void PifeWebSocket::enviarMensagem(crow::websocket::connection& conn, const crow::json::wvalue& mensagem) {
    conn.send_text(mensagem.dump());
}

void PifeWebSocket::enviarErro(crow::websocket::connection& conn, const std::string& mensagem) {
    crow::json::wvalue erro;
    erro["tipo"] = "erro";
    erro["erro"] = mensagem;
    erro["mensagem"] = mensagem;
    enviarMensagem(conn, erro);
}

bool PifeWebSocket::tokenValido(const std::string& tokenReconexao) {
    return tokenReconexao.size() >= 8 && tokenReconexao.size() <= 100;
}

void PifeWebSocket::adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta) {
    destino["valor"] = static_cast<int>(carta.mostraValor());
    destino["naipe"] = static_cast<int>(carta.mostraNaipe());
}

void PifeWebSocket::enviarEstadoJogador(SalaPife* sala, int idJogador, crow::websocket::connection& conn) {
    if (!sala) return;

    Pife& jogo = sala->jogo();

    if (idJogador < 0 || idJogador >= jogo.numeroDeJogadores())
        return enviarErro(conn, "Jogador inválido");

    crow::json::wvalue estado;

    estado["tipo"] = "estado_jogo";
    estado["sala"] = sala->idSala();
    estado["meu_id"] = idJogador;
    estado["max_jogadores"] = sala->maxJogadores();
    estado["jogadores_conectados"] = sala->jogadoresConectados();
    estado["jogadores_registrados"] = sala->jogadoresRegistrados();
    estado["partida_iniciada"] = sala->partidaIniciada();
    estado["jogador_atual"] = jogo.consultarIndiceJogadorAtual();
    estado["jogo_finalizado"] = jogo.jogoFinalizado();
    estado["vencedor"] = jogo.consultarVencedor();
    estado["quantidade_baralho"] = jogo.quantidadeCartasBaralho();

    const JogadorPife& jogadorLocal = jogo.consultarJogador(idJogador);

    estado["meu_nome"] = jogadorLocal.nome().empty()
        ? "Jogador " + std::to_string(idJogador + 1)
        : jogadorLocal.nome();

    FaseTurno fase = jogo.consultarFaseTurno();

    if (fase == FaseTurno::AguardandoCompra)
        estado["fase"] = "AGUARDANDO_COMPRA";
    else if (fase == FaseTurno::AguardandoDescarte)
        estado["fase"] = "AGUARDANDO_DESCARTE";
    else
        estado["fase"] = "FINALIZADO";

    bool partidaDisponivel = sala->partidaIniciada() && !jogo.jogoFinalizado();

    estado["pode_comprar_baralho"] = partidaDisponivel && jogo.podeComprarBaralho(idJogador);
    estado["pode_comprar_mesa"] = partidaDisponivel && jogo.podeComprarMesa(idJogador);
    estado["pode_descartar"] = partidaDisponivel && jogo.podeColocarNaMesa(idJogador);
    estado["pode_bater"] = partidaDisponivel && jogo.podeBater(idJogador);

    const std::vector<Carta>& mao = jogo.consultarMao(idJogador);
    estado["minha_mao"] = crow::json::wvalue::list();

    for (std::size_t i = 0; i < mao.size(); ++i)
        adicionarCartaAoJson(estado["minha_mao"][i], mao[i]);

    const std::vector<Carta>& mesa = jogo.consultarMesa();
    estado["mesa"] = crow::json::wvalue::list();

    for (std::size_t i = 0; i < mesa.size(); ++i)
        adicionarCartaAoJson(estado["mesa"][i], mesa[i]);

    adicionarCartaAoJson(estado["vira"], jogo.consultarVira());

    estado["jogadores"] = crow::json::wvalue::list();

    for (int i = 0; i < jogo.numeroDeJogadores(); ++i) {
        const JogadorPife& jogador = jogo.consultarJogador(i);

        estado["jogadores"][i]["id"] = i;
        estado["jogadores"][i]["nome"] = jogador.nome().empty()
            ? "Jogador " + std::to_string(i + 1)
            : jogador.nome();
        estado["jogadores"][i]["quantidade_cartas"] = jogador.tmnhMao();
        estado["jogadores"][i]["sou_eu"] = i == idJogador;
        estado["jogadores"][i]["conectado"] = sala->jogadorEstaConectado(i);
    }

    enviarMensagem(conn, estado);
}

void PifeWebSocket::enviarEstadoSala(SalaPife* sala) {
    if (!sala) return;

    for (const ConexaoPife& registro : sala->conexoes()) {
        if (registro.conexao)
            enviarEstadoJogador(sala, registro.idJogador, *registro.conexao);
    }
}

void PifeWebSocket::removerConexao(crow::websocket::connection& conn) {
    auto sessaoIt = sessoes_.find(&conn);

    if (sessaoIt == sessoes_.end())
        return;

    std::string idSala = sessaoIt->second.idSala;
    sessoes_.erase(sessaoIt);

    SalaPife* sala = GerenciadorSalasPife::obterSala(idSala);

    if (!sala)
        return;

    sala->removerConexao(&conn);
    enviarEstadoSala(sala);
}