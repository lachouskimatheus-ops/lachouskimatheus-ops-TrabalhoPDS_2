#include "multiplayer/Pife/PifeWebSocket.hpp"

#include "multiplayer/Pife/GerenciadorSalasPife.hpp"

std::map<crow::websocket::connection*, SessaoPife> PifeWebSocket::sessoes_;

void PifeWebSocket::registrar(crow::SimpleApp& app) {
    CROW_WEBSOCKET_ROUTE(app, "/ws/pife")

    .onopen([](crow::websocket::connection& conn) {
        crow::json::wvalue resposta;
        resposta["tipo"] = "conectado";
        resposta["mensagem"] = "Conectado ao servidor do Pife";

        enviarMensagem(conn, resposta);
    })

    .onmessage([](crow::websocket::connection& conn, const std::string& data, bool isBinary) {
        if (isBinary) {
            enviarErro(conn, "Mensagens binárias não são aceitas");
            return;
        }

        auto dados = crow::json::load(data);

        if (!dados) {
            enviarErro(conn, "Mensagem JSON inválida");
            return;
        }

        if (!dados.has("tipo")) {
            enviarErro(conn, "A mensagem não possui o campo tipo");
            return;
        }

        std::string tipo = dados["tipo"].s();

        if (tipo == "entrar_sala") {
            entrarNaSala(conn, dados);
            return;
        }

        if (tipo == "acao_jogo") {
            processarAcao(conn, dados);
            return;
        }

        if (tipo == "obter_estado") {
            auto sessaoIt = sessoes_.find(&conn);

            if (sessaoIt == sessoes_.end()) {
                enviarErro(conn, "Você ainda não entrou em uma sala");
                return;
            }

            SalaPife* sala = GerenciadorSalasPife::obterSala(sessaoIt->second.idSala);

            if (sala == nullptr) {
                enviarErro(conn, "Sala não encontrada");
                return;
            }

            enviarEstadoJogador(sala, sessaoIt->second.idJogador, conn);
            return;
        }

        enviarErro(conn, "Tipo de mensagem não reconhecido");
    })

    .onclose([](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
        (void)reason;
        (void)code;

        removerConexao(conn);
    });
}

void PifeWebSocket::entrarNaSala(crow::websocket::connection& conn, const crow::json::rvalue& dados) {
    if (sessoes_.find(&conn) != sessoes_.end()) {
        enviarErro(conn, "Esta conexão já entrou em uma sala");
        return;
    }

    if (!dados.has("sala")) {
        enviarErro(conn, "Informe o código da sala");
        return;
    }

    std::string idSala = dados["sala"].s();

    if (idSala.empty()) {
        enviarErro(conn, "O código da sala é inválido");
        return;
    }

    SalaPife* sala = GerenciadorSalasPife::obterSala(idSala);

    if (sala == nullptr) {
        enviarErro(conn, "Sala não encontrada");
        return;
    }

    if (sala->partidaIniciada()) {
        enviarErro(conn, "A partida desta sala já foi iniciada");
        return;
    }

    if (sala->estaCheia()) {
        enviarErro(conn, "A sala está cheia");
        return;
    }

    int idJogador = sala->adicionarJogador(&conn);

    if (idJogador == -1) {
        enviarErro(conn, "Não foi possível entrar na sala");
        return;
    }

    sessoes_[&conn] = {idSala, idJogador};

    crow::json::wvalue resposta;
    resposta["tipo"] = "entrada_confirmada";
    resposta["sala"] = idSala;
    resposta["idJogador"] = idJogador;
    resposta["maxJogadores"] = sala->maxJogadores();
    resposta["jogadoresConectados"] = sala->jogadoresConectados();
    resposta["partidaIniciada"] = sala->partidaIniciada();

    enviarMensagem(conn, resposta);
    enviarEstadoSala(sala);
}

void PifeWebSocket::processarAcao(crow::websocket::connection& conn, const crow::json::rvalue& dados) {
    auto sessaoIt = sessoes_.find(&conn);

    if (sessaoIt == sessoes_.end()) {
        enviarErro(conn, "Você ainda não entrou em uma sala");
        return;
    }

    if (!dados.has("acao")) {
        enviarErro(conn, "A mensagem não possui o campo acao");
        return;
    }

    const SessaoPife& sessao = sessaoIt->second;

    SalaPife* sala = GerenciadorSalasPife::obterSala(sessao.idSala);

    if (sala == nullptr) {
        enviarErro(conn, "Sala não encontrada");
        return;
    }

    if (!sala->partidaIniciada()) {
        enviarErro(conn, "Aguardando os outros jogadores entrarem");
        return;
    }

    Pife& jogo = sala->jogo();

    if (jogo.jogoFinalizado()) {
        enviarErro(conn, "A partida já foi finalizada");
        return;
    }

    int idJogador = sessao.idJogador;
    std::string acao = dados["acao"].s();

    bool sucesso = false;
    std::string mensagemErro;

    if (acao == "COMPRAR_BARALHO") {
        sucesso = jogo.comprarBaralho(idJogador);

        if (!sucesso) {
            mensagemErro = "Não é possível comprar do baralho agora";
        }
    }
    else if (acao == "COMPRAR_MESA") {
        sucesso = jogo.comprarMesa(idJogador);

        if (!sucesso) {
            mensagemErro = "Não é possível comprar da mesa agora";
        }
    }
    else if (acao == "DESCARTAR") {
        if (!dados.has("indice")) {
            enviarErro(conn, "Informe o índice da carta");
            return;
        }

        int indice = dados["indice"].i();

        sucesso = jogo.colocarNaMesa(idJogador, indice);

        if (!sucesso) {
            mensagemErro = "Não é possível descartar esta carta agora";
        }
    }
    else if (acao == "ORGANIZAR") {
        sucesso = jogo.organizarMao(idJogador);

        if (!sucesso) {
            mensagemErro = "Não foi possível organizar a mão";
        }
    }
    else if (acao == "BATER") {
        sucesso = jogo.bati(idJogador);

        if (!sucesso) {
            mensagemErro = "Sua mão ainda não forma uma combinação vencedora";
        }
    }
    else {
        enviarErro(conn, "Ação do Pife não reconhecida");
        return;
    }

    if (!sucesso) {
        enviarErro(conn, mensagemErro);
        enviarEstadoJogador(sala, idJogador, conn);
        return;
    }

    enviarEstadoSala(sala);
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

void PifeWebSocket::adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta) {
    destino["valor"] = static_cast<int>(carta.mostraValor());
    destino["naipe"] = static_cast<int>(carta.mostraNaipe());
}

void PifeWebSocket::enviarEstadoJogador(SalaPife* sala, int idJogador, crow::websocket::connection& conn) {
    if (sala == nullptr) {
        return;
    }

    Pife& jogo = sala->jogo();

    crow::json::wvalue estado;

    estado["tipo"] = "estado_jogo";
    estado["sala"] = sala->idSala();
    estado["meu_id"] = idJogador;
    estado["max_jogadores"] = sala->maxJogadores();
    estado["jogadores_conectados"] = sala->jogadoresConectados();
    estado["partida_iniciada"] = sala->partidaIniciada();
    estado["jogador_atual"] = jogo.consultarIndiceJogadorAtual();
    estado["jogo_finalizado"] = jogo.jogoFinalizado();
    estado["vencedor"] = jogo.consultarVencedor();
    estado["quantidade_baralho"] = jogo.quantidadeCartasBaralho();

    FaseTurno fase = jogo.consultarFaseTurno();

    if (fase == FaseTurno::AguardandoCompra) {
        estado["fase"] = "AGUARDANDO_COMPRA";
    }
    else if (fase == FaseTurno::AguardandoDescarte) {
        estado["fase"] = "AGUARDANDO_DESCARTE";
    }
    else {
        estado["fase"] = "FINALIZADO";
    }

    bool partidaDisponivel = sala->partidaIniciada() && !jogo.jogoFinalizado();

    estado["pode_comprar_baralho"] = partidaDisponivel && jogo.podeComprarBaralho(idJogador);
    estado["pode_comprar_mesa"] = partidaDisponivel && jogo.podeComprarMesa(idJogador);
    estado["pode_descartar"] = partidaDisponivel && jogo.podeColocarNaMesa(idJogador);
    estado["pode_bater"] = partidaDisponivel && jogo.podeBater(idJogador);

    const std::vector<Carta>& mao = jogo.consultarMao(idJogador);

    estado["minha_mao"] = crow::json::wvalue::list();

    for (std::size_t i = 0; i < mao.size(); i++) {
        adicionarCartaAoJson(estado["minha_mao"][i], mao[i]);
    }

    const std::vector<Carta>& mesa = jogo.consultarMesa();

    estado["mesa"] = crow::json::wvalue::list();

    for (std::size_t i = 0; i < mesa.size(); i++) {
        adicionarCartaAoJson(estado["mesa"][i], mesa[i]);
    }

    const Carta& vira = jogo.consultarVira();

    adicionarCartaAoJson(estado["vira"], vira);

    estado["jogadores"] = crow::json::wvalue::list();

    for (int i = 0; i < jogo.numeroDeJogadores(); i++) {
        estado["jogadores"][i]["id"] = i;
        estado["jogadores"][i]["quantidade_cartas"] = jogo.consultarJogador(i).tmnhMao();
        estado["jogadores"][i]["sou_eu"] = i == idJogador;
        estado["jogadores"][i]["conectado"] = false;

        for (const JogadorConectado& jogador : sala->jogadores()) {
            if (jogador.idJogador == i) {
                estado["jogadores"][i]["conectado"] = jogador.conectado;
                break;
            }
        }
    }

    enviarMensagem(conn, estado);
}

void PifeWebSocket::enviarEstadoSala(SalaPife* sala) {
    if (sala == nullptr) {
        return;
    }

    for (const ConexaoPife& conexao : sala->conexoes()) {
        if (conexao.conexao == nullptr) {
            continue;
        }

        enviarEstadoJogador(sala, conexao.idJogador, *conexao.conexao);
    }
}

void PifeWebSocket::removerConexao(crow::websocket::connection& conn) {
    auto sessaoIt = sessoes_.find(&conn);

    if (sessaoIt == sessoes_.end()) {
        return;
    }

    std::string idSala = sessaoIt->second.idSala;

    sessoes_.erase(sessaoIt);

    SalaPife* sala = GerenciadorSalasPife::obterSala(idSala);

    if (sala == nullptr) {
        return;
    }

    sala->removerConexao(&conn);

    if (sala->estaVazia()) {
        GerenciadorSalasPife::removerSala(idSala);
        return;
    }

    enviarEstadoSala(sala);
}