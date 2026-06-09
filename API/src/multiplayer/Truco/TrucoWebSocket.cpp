#include "multiplayer/Truco/TrucoWebSocket.hpp"

std::map<crow::websocket::connection*, SessaoWebSocket> TrucoWebSocket::sessoes_;

void TrucoWebSocket::registrar(crow::SimpleApp& app) {
    CROW_WEBSOCKET_ROUTE(app, "/ws/truco")
    .onopen([](crow::websocket::connection& conexao) {
        crow::json::wvalue resposta;
        resposta["tipo"] = "conectado";
        resposta["mensagem"] = "Conectado ao servidor do Truco";
        enviarMensagem(conexao, resposta);
    })
    .onmessage([](crow::websocket::connection& conexao, const std::string& dadosTexto, bool binario) {
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

        if (tipo == "entrar_sala") entrarNaSala(conexao, dados);
        else if (tipo == "acao_jogo") processarAcao(conexao, dados);
        else if (tipo == "ping") processarPing(conexao);
        else if (tipo == "obter_estado") {
            auto it = sessoes_.find(&conexao);

            if (it == sessoes_.end()) {
                enviarErro(conexao, "Você ainda não entrou em uma sala");
                return;
            }

            SalaTruco* sala = GerenciadorSalasTruco::obterSala(it->second.idSala);

            if (sala == nullptr) {
                enviarErro(conexao, "Sala não encontrada");
                return;
            }

            enviarEstadoJogador(sala, it->second.idJogador, conexao);
        } else {
            enviarErro(conexao, "Tipo de mensagem não reconhecido");
        }
    })
    .onclose([](crow::websocket::connection& conexao, const std::string& motivo, uint16_t codigo) {
        (void)motivo;
        (void)codigo;
        removerConexao(conexao);
    });
}

void TrucoWebSocket::entrarNaSala(crow::websocket::connection& conexao, const crow::json::rvalue& dados) {
    if (sessoes_.find(&conexao) != sessoes_.end()) {
        enviarErro(conexao, "Esta conexão já entrou em uma sala");
        return;
    }

    if (!dados.has("sala") || !dados.has("token")) {
        enviarErro(conexao, "Informe sala e token de reconexão");
        return;
    }

    std::string idSala = dados["sala"].s();
    std::string token = dados["token"].s();

    if (idSala.empty() || !tokenValido(token)) {
        enviarErro(conexao, "Sala ou token inválido");
        return;
    }

    SalaTruco* sala = GerenciadorSalasTruco::obterSala(idSala);

    if (sala == nullptr) {
        enviarErro(conexao, "Sala não encontrada");
        return;
    }

    bool reconexao = sala->podeReconectar(token);

    if (sala->partidaIniciada() && !reconexao) {
        enviarErro(conexao, "A partida desta sala já foi iniciada");
        return;
    }

    crow::websocket::connection* conexaoAnterior = nullptr;
    int idJogador = -1;

    if (reconexao) {
        int idAnterior = sala->buscarJogadorPorToken(token);
        conexaoAnterior = sala->obterConexaoJogador(idAnterior);
        idJogador = sala->reconectarJogador(&conexao, token);
    } else {
        if (!dados.has("nome") || !dados.has("equipe")) {
            enviarErro(conexao, "Informe nome e equipe");
            return;
        }

        std::string nome = dados["nome"].s();
        int equipe = dados["equipe"].i();

        if (!sala->podeReceberNovoJogador()) {
            enviarErro(conexao, "A sala está cheia");
            return;
        }

        if (!sala->equipeDisponivel(equipe)) {
            enviarErro(conexao, "A equipe escolhida está cheia ou é inválida");
            return;
        }

        idJogador = sala->adicionarJogador(&conexao, token, nome, equipe);
    }

    if (idJogador == -1) {
        enviarErro(conexao, "Não foi possível entrar na sala");
        return;
    }

    if (conexaoAnterior != nullptr && conexaoAnterior != &conexao) {
        sessoes_.erase(conexaoAnterior);

        crow::json::wvalue aviso;
        aviso["tipo"] = "sessao_substituida";
        aviso["mensagem"] = "Sua sessão foi aberta em outra conexão";

        enviarMensagem(*conexaoAnterior, aviso);
        conexaoAnterior->close("Sessão substituída");
    }

    sessoes_[&conexao] = {idSala, idJogador};

    crow::json::wvalue resposta;
    resposta["tipo"] = reconexao ? "reconectado_sala" : "entrou_sala";
    resposta["sala"] = idSala;
    resposta["id_jogador"] = idJogador;
    resposta["partida_iniciada"] = sala->partidaIniciada();

    enviarMensagem(conexao, resposta);
    enviarEstadoSala(sala);
}

void TrucoWebSocket::processarAcao(crow::websocket::connection& conexao, const crow::json::rvalue& dados) {
    auto it = sessoes_.find(&conexao);

    if (it == sessoes_.end()) {
        enviarErro(conexao, "Você ainda não entrou em uma sala");
        return;
    }

    if (!dados.has("acao")) {
        enviarErro(conexao, "Informe a ação");
        return;
    }

    SalaTruco* sala = GerenciadorSalasTruco::obterSala(it->second.idSala);

    if (sala == nullptr) {
        enviarErro(conexao, "Sala não encontrada");
        return;
    }

    if (!sala->partidaIniciada()) {
        enviarErro(conexao, "Aguardando os outros jogadores");
        return;
    }

    std::string acao = dados["acao"].s();
    int idJogador = it->second.idJogador;

    if (acao == "JOGAR_CARTA") jogarCarta(conexao, *sala, idJogador, dados);
    else if (acao == "PEDIR_TRUCO") pedirTruco(conexao, *sala, idJogador);
    else if (acao == "ACEITAR_TRUCO") aceitarTruco(conexao, *sala, idJogador);
    else if (acao == "RECUSAR_TRUCO") recusarTruco(conexao, *sala, idJogador);
    else if (acao == "AUMENTAR_TRUCO") aumentarTruco(conexao, *sala, idJogador);
    else if (acao == "NOVA_MAO") iniciarNovaMao(conexao, *sala);
    else enviarErro(conexao, "Ação do Truco não reconhecida");
}

void TrucoWebSocket::jogarCarta(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador, const crow::json::rvalue& dados) {
    if (!dados.has("indice")) {
        enviarErro(conexao, "Informe o índice da carta");
        return;
    }

    int indice = dados["indice"].i();

    if (!sala.jogo().jogarCarta(idJogador, indice)) {
        enviarErro(conexao, sala.jogo().getUltimaMensagem());
        return;
    }

    enviarEstadoSala(&sala);
}

void TrucoWebSocket::pedirTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador) {
    if (!sala.jogo().pedirTruco(idJogador)) {
        enviarErro(conexao, sala.jogo().getUltimaMensagem());
        return;
    }

    enviarEstadoSala(&sala);
}

void TrucoWebSocket::aceitarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador) {
    if (!sala.jogo().aceitarTruco(idJogador)) {
        enviarErro(conexao, sala.jogo().getUltimaMensagem());
        return;
    }

    enviarEstadoSala(&sala);
}

void TrucoWebSocket::recusarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador) {
    if (!sala.jogo().recusarTruco(idJogador)) {
        enviarErro(conexao, sala.jogo().getUltimaMensagem());
        return;
    }

    enviarEstadoSala(&sala);
}

void TrucoWebSocket::aumentarTruco(crow::websocket::connection& conexao, SalaTruco& sala, int idJogador) {
    if (!sala.jogo().aumentarTruco(idJogador)) {
        enviarErro(conexao, sala.jogo().getUltimaMensagem());
        return;
    }

    enviarEstadoSala(&sala);
}

void TrucoWebSocket::iniciarNovaMao(crow::websocket::connection& conexao, SalaTruco& sala) {
    if (sala.jogo().getFase() != FaseTruco::MaoFinalizada) {
        enviarErro(conexao, "A mão atual ainda não terminou");
        return;
    }

    if (!sala.jogo().iniciarNovaMao()) {
        enviarErro(conexao, sala.jogo().getUltimaMensagem());
        return;
    }

    enviarEstadoSala(&sala);
}

void TrucoWebSocket::processarPing(crow::websocket::connection& conexao) {
    crow::json::wvalue resposta;
    resposta["tipo"] = "pong";

    enviarMensagem(conexao, resposta);
}

void TrucoWebSocket::adicionarCartaAoJson(crow::json::wvalue& destino, const Carta& carta) {
    destino["valor"] = static_cast<int>(carta.getValor());
    destino["naipe"] = static_cast<int>(carta.getNaipe());
}

void TrucoWebSocket::enviarMensagem(crow::websocket::connection& conexao, const crow::json::wvalue& mensagem) {
    conexao.send_text(mensagem.dump());
}

void TrucoWebSocket::enviarErro(crow::websocket::connection& conexao, const std::string& mensagem) {
    crow::json::wvalue erro;
    erro["tipo"] = "erro";
    erro["mensagem"] = mensagem;

    enviarMensagem(conexao, erro);
}

void TrucoWebSocket::enviarEstadoJogador(SalaTruco* sala, int idJogador, crow::websocket::connection& conexao) {
    if (sala == nullptr) return;

    Truco& jogo = sala->jogo();

    crow::json::wvalue estado;
    estado["tipo"] = "estado_jogo";
    estado["sala"] = sala->idSala();
    estado["tipo_truco"] = tipoParaString(sala->tipo());
    estado["meu_id"] = idJogador;
    estado["minha_equipe"] = jogo.getEquipeDoJogador(idJogador);
    estado["max_jogadores"] = sala->maxJogadores();
    estado["jogadores_conectados"] = sala->jogadoresConectados();
    estado["jogadores_registrados"] = sala->jogadoresRegistrados();
    estado["partida_iniciada"] = sala->partidaIniciada();
    estado["fase"] = faseParaString(jogo.getFase());
    estado["jogador_atual"] = jogo.getJogadorAtual();
    estado["numero_queda"] = jogo.getNumeroQueda();
    estado["valor_mao"] = jogo.getValorMao();
    estado["pontos_equipe_1"] = jogo.getPontosEquipe1();
    estado["pontos_equipe_2"] = jogo.getPontosEquipe2();
    estado["vencedor_ultima_queda"] = jogo.getVencedorUltimaQueda();
    estado["vencedor_mao"] = jogo.getVencedorMao();
    estado["vencedor_partida"] = jogo.getVencedorPartida();
    estado["equipe_que_pediu"] = jogo.getEquipeQuePediu();
    estado["valor_pedido"] = jogo.getValorPedido();
    estado["mao_de_onze"] = jogo.getMaoTravada();
    estado["nome_pedidor"] = jogo.getNomePedidor();
    estado["ultimo_evento"] = jogo.getUltimoEvento();
    estado["mensagem"] = jogo.getUltimaMensagem();
    estado["pode_jogar"] = jogo.podeJogar(idJogador);
    estado["pode_pedir_truco"] = jogo.podePedirTruco(idJogador);
    estado["pode_responder_truco"] = jogo.podeResponderTruco(idJogador);

    if (jogo.getVira() != nullptr) {
        adicionarCartaAoJson(estado["vira"], *jogo.getVira());
    }

    estado["minha_mao"] = crow::json::wvalue::list();

    const Jogador_Truco* jogadorLocal = sala->jogadorTruco(idJogador);

    if (jogadorLocal != nullptr) {
        const auto& mao = jogadorLocal->getMao();

        for (std::size_t i = 0; i < mao.size(); ++i) {
            if (mao[i] != nullptr) {
                adicionarCartaAoJson(estado["minha_mao"][i], *mao[i]);
            }
        }
    }

    estado["jogadas_queda"] = crow::json::wvalue::list();

    const auto& jogadas = jogo.getJogadasDaQueda();

    for (std::size_t i = 0; i < jogadas.size(); ++i) {
        estado["jogadas_queda"][i]["id_jogador"] = jogadas[i].idJogador;

        if (jogadas[i].carta != nullptr) {
            adicionarCartaAoJson(estado["jogadas_queda"][i]["carta"], *jogadas[i].carta);
        }
    }

    estado["jogadores"] = crow::json::wvalue::list();

    const auto& jogadores = jogo.getJogadores();

    for (std::size_t i = 0; i < jogadores.size(); ++i) {
        int id = static_cast<int>(i);

        estado["jogadores"][i]["id"] = id;
        estado["jogadores"][i]["nome"] = jogadores[i] == nullptr ? "" : jogadores[i]->getNome();
        estado["jogadores"][i]["equipe"] = jogo.getEquipeDoJogador(id);
        estado["jogadores"][i]["sou_eu"] = id == idJogador;
        estado["jogadores"][i]["conectado"] = sala->jogadorEstaConectado(id);
        estado["jogadores"][i]["quantidade_cartas"] =
            jogadores[i] == nullptr ? 0 : static_cast<int>(jogadores[i]->getMao().size());
    }

    enviarMensagem(conexao, estado);
}

void TrucoWebSocket::enviarEstadoSala(SalaTruco* sala) {
    if (sala == nullptr) return;

    for (const ConexaoTruco& registro : sala->conexoes()) {
        if (registro.conexao != nullptr) {
            enviarEstadoJogador(sala, registro.idJogador, *registro.conexao);
        }
    }
}

std::string TrucoWebSocket::faseParaString(FaseTruco fase) {
    switch (fase) {
        case FaseTruco::AguardandoInicio:
            return "AGUARDANDO_INICIO";

        case FaseTruco::AguardandoJogada:
            return "AGUARDANDO_JOGADA";

        case FaseTruco::AguardandoRespostaTruco:
            return "AGUARDANDO_RESPOSTA_TRUCO";

        case FaseTruco::MaoFinalizada:
            return "MAO_FINALIZADA";

        case FaseTruco::PartidaFinalizada:
            return "PARTIDA_FINALIZADA";
    }

    return "DESCONHECIDA";
}

std::string TrucoWebSocket::tipoParaString(TipoTruco tipo) {
    return tipo == TipoTruco::Mineiro ? "MINEIRO" : "PAULISTA";
}

bool TrucoWebSocket::tokenValido(const std::string& tokenReconexao) {
    return tokenReconexao.size() >= 8 && tokenReconexao.size() <= 100;
}

void TrucoWebSocket::removerConexao(crow::websocket::connection& conexao) {
    auto it = sessoes_.find(&conexao);

    if (it == sessoes_.end()) return;

    std::string idSala = it->second.idSala;
    sessoes_.erase(it);

    SalaTruco* sala = GerenciadorSalasTruco::obterSala(idSala);

    if (sala == nullptr) return;

    sala->removerConexao(&conexao);
    enviarEstadoSala(sala);
}