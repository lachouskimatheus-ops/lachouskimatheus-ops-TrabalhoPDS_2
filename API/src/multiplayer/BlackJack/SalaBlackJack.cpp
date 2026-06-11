#include "multiplayer/BlackJack/SalaBlackJack.hpp"

#include <algorithm>

SalaBlackJack::SalaBlackJack(const std::string& idSala, int maxJogadores)
    : SalaBase(idSala, maxJogadores),
      partidaIniciada_(false) {
    nomes_.resize(maxJogadores, "");
}

ConexaoBlackJack* SalaBlackJack::buscarConexaoDoJogador(int idJogador) {
    for (ConexaoBlackJack& registro : conexoes_) {
        if (registro.idJogador == idJogador) {
            return &registro;
        }
    }

    return nullptr;
}

const ConexaoBlackJack* SalaBlackJack::buscarConexaoDoJogador(int idJogador) const {
    for (const ConexaoBlackJack& registro : conexoes_) {
        if (registro.idJogador == idJogador) {
            return &registro;
        }
    }

    return nullptr;
}

int SalaBlackJack::idLogicoJogo(int idSalaBase) const {
    // SalaBase usa IDs 0, 1, 2...
    // Jogo21 usa IDs 1, 2, 3... porque a banca fica com ID 0.
    return idSalaBase + 1;
}

int SalaBlackJack::adicionarJogador(crow::websocket::connection* conexao,
                                    const std::string& tokenReconexao,
                                    const std::string& nome) {
    if (conexao == nullptr || tokenReconexao.empty()) {
        return -1;
    }

    int idExistente = obterIdJogador(conexao);

    if (idExistente != -1) {
        return idExistente;
    }

    if (tokenExiste(tokenReconexao)) {
        return reconectarJogador(conexao, tokenReconexao);
    }

    if (!podeReceberNovoJogador()) {
        return -1;
    }

    int idJogador = SalaBase::adicionarJogador(tokenReconexao);

    if (idJogador == -1) {
        return -1;
    }

    conexoes_.push_back({idJogador, conexao});

    if (idJogador >= 0 && idJogador < static_cast<int>(nomes_.size())) {
        nomes_[idJogador] = nome.empty()
            ? "Jogador " + std::to_string(idJogador + 1)
            : nome;
    }

    if (todosConectados()) {
        iniciarPartida();
    }

    return idJogador;
}

int SalaBlackJack::reconectarJogador(crow::websocket::connection* conexao,
                                     const std::string& tokenReconexao) {
    if (conexao == nullptr || tokenReconexao.empty()) {
        return -1;
    }

    int idJogador = SalaBase::reconectarJogador(tokenReconexao);

    if (idJogador == -1) {
        return -1;
    }

    ConexaoBlackJack* registro = buscarConexaoDoJogador(idJogador);

    if (registro != nullptr) {
        registro->conexao = conexao;
    } else {
        conexoes_.push_back({idJogador, conexao});
    }

    return idJogador;
}

bool SalaBlackJack::removerConexao(crow::websocket::connection* conexao) {
    if (conexao == nullptr) {
        return false;
    }

    for (auto it = conexoes_.begin(); it != conexoes_.end(); ++it) {
        if (it->conexao != conexao) {
            continue;
        }

        int idJogador = it->idJogador;
        conexoes_.erase(it);

        return SalaBase::desconectarJogador(idJogador);
    }

    return false;
}

int SalaBlackJack::obterIdJogador(crow::websocket::connection* conexao) const {
    if (conexao == nullptr) {
        return -1;
    }

    for (const ConexaoBlackJack& registro : conexoes_) {
        if (registro.conexao == conexao) {
            return registro.idJogador;
        }
    }

    return -1;
}

crow::websocket::connection* SalaBlackJack::obterConexaoJogador(int idJogador) const {
    const ConexaoBlackJack* registro = buscarConexaoDoJogador(idJogador);

    if (registro == nullptr) {
        return nullptr;
    }

    return registro->conexao;
}

bool SalaBlackJack::possuiConexao(crow::websocket::connection* conexao) const {
    return obterIdJogador(conexao) != -1;
}

bool SalaBlackJack::podeReceberNovoJogador() const {
    return !partidaIniciada_ && podeAdicionarNovoJogador();
}

bool SalaBlackJack::podeReconectar(const std::string& tokenReconexao) const {
    return !tokenReconexao.empty() && tokenExiste(tokenReconexao);
}

bool SalaBlackJack::iniciarPartida() {
    if (partidaIniciada_) {
        return true;
    }

    if (!todosConectados()) {
        return false;
    }

    std::vector<std::string> nomesValidos;

    for (int i = 0; i < maxJogadores(); i++) {
        if (i < static_cast<int>(nomes_.size()) && !nomes_[i].empty()) {
            nomesValidos.push_back(nomes_[i]);
        } else {
            nomesValidos.push_back("Jogador " + std::to_string(i + 1));
        }
    }

    if (!jogo_.inicializarJogo(nomesValidos)) {
        return false;
    }

    partidaIniciada_ = true;
    return true;
}

bool SalaBlackJack::partidaIniciada() const {
    return partidaIniciada_;
}

bool SalaBlackJack::pedirCarta(int idJogador) {
    if (!partidaIniciada_) {
        return false;
    }

    return jogo_.pedirCarta(idLogicoJogo(idJogador));
}

bool SalaBlackJack::parar(int idJogador) {
    if (!partidaIniciada_) {
        return false;
    }

    return jogo_.parar(idLogicoJogo(idJogador));
}

bool SalaBlackJack::iniciarNovaRodada(int idSolicitante) {
    if (!partidaIniciada_) {
        return false;
    }

    // Para manter simples, só o primeiro jogador inicia a próxima rodada.
    if (idSolicitante != 0) {
        return false;
    }

    return jogo_.iniciarNovaRodada();
}

std::string SalaBlackJack::faseParaString(Fase21 fase) {
    switch (fase) {
        case Fase21::NaoIniciado:
            return "NAO_INICIADO";

        case Fase21::EscolhendoAcoes:
            return "ESCOLHENDO_ACOES";

        case Fase21::TurnoBanca:
            return "TURNO_BANCA";

        case Fase21::Resultado:
            return "RESULTADO";

        default:
            return "DESCONHECIDA";
    }
}

std::string SalaBlackJack::resultadoParaString(Resultado21 resultado) {
    switch (resultado) {
        case Resultado21::Vitoria:
            return "VITORIA";

        case Resultado21::Derrota:
            return "DERROTA";

        case Resultado21::Empate:
            return "EMPATE";

        case Resultado21::Estourou:
            return "ESTOUROU";

        case Resultado21::Indefinido:
        default:
            return "INDEFINIDO";
    }
}

json SalaBlackJack::cartaParaJson(const Carta* carta, bool oculta) const {
    if (carta == nullptr || oculta) {
        return {
            {"valor", 0},
            {"naipe", 0},
            {"texto", "Carta oculta"},
            {"oculta", true}
        };
    }

    return {
        {"valor", carta->getValor()},
        {"naipe", static_cast<int>(carta->getNaipe())},
        {"texto", carta->cartaString()},
        {"oculta", false}
    };
}

json SalaBlackJack::maoParaJson(const std::vector<Carta*>& mao, bool ocultarSegundaCarta) const {
    json cartas = json::array();

    for (int i = 0; i < static_cast<int>(mao.size()); i++) {
        bool oculta = ocultarSegundaCarta && i == 1;
        cartas.push_back(cartaParaJson(mao[i], oculta));
    }

    return cartas;
}

json SalaBlackJack::gerarJson(int idJogador) const {
    json estado;

    bool iniciou = partidaIniciada_;
    Fase21 faseAtual = iniciou ? jogo_.fase() : Fase21::NaoIniciado;

    estado["tipo"] = "estado_jogo";
    estado["sala"] = idSala();
    estado["meu_id"] = idJogador;
    estado["jogadores_conectados"] = jogadoresConectados();
    estado["jogadores_registrados"] = jogadoresRegistrados();
    estado["max_jogadores"] = maxJogadores();
    estado["partida_iniciada"] = iniciou;
    estado["fase"] = faseParaString(faseAtual);
    estado["rodada"] = iniciou ? jogo_.rodada() : 0;

    estado["mensagem"] = iniciou
        ? jogo_.nomeFase()
        : "Aguardando jogadores entrarem na mesa.";

    estado["jogadores"] = json::array();

    const std::vector<Jogador21*>& jogadoresJogo = jogo_.getJogadores();

    for (int i = 0; i < maxJogadores(); i++) {
        json jogadorJson;

        jogadorJson["id"] = i;
        jogadorJson["nome"] =
            (i < static_cast<int>(nomes_.size()) && !nomes_[i].empty())
                ? nomes_[i]
                : "Jogador " + std::to_string(i + 1);

        jogadorJson["conectado"] = jogadorEstaConectado(i);
        jogadorJson["pode_agir"] = iniciou && jogo_.podeAgir(idLogicoJogo(i));
        jogadorJson["resultado"] = "INDEFINIDO";
        jogadorJson["pontuacao"] = 0;
        jogadorJson["estourou"] = false;
        jogadorJson["parou"] = false;
        jogadorJson["mao"] = json::array();

        if (iniciou && i < static_cast<int>(jogadoresJogo.size())) {
            Jogador21* jogador = jogadoresJogo[i];

            if (jogador != nullptr) {
                jogadorJson["pontuacao"] = jogador->calcularPontuacao();
                jogadorJson["estourou"] = jogador->estourou();
                jogadorJson["parou"] = jogador->parou();

                if (faseAtual == Fase21::Resultado) {
                    jogadorJson["resultado"] =
                        resultadoParaString(jogo_.resultadoJogador(idLogicoJogo(i)));
                }

                jogadorJson["mao"] = maoParaJson(jogador->verMao(), false);
            }
        }

        estado["jogadores"].push_back(jogadorJson);
    }

    estado["banca"] = {
        {"nome", "Banca"},
        {"pontuacao", 0},
        {"estourou", false},
        {"mao", json::array()}
    };

    if (iniciou && jogo_.getBanca() != nullptr) {
        Jogador21* banca = jogo_.getBanca();

        bool esconderSegundaCarta =
            faseAtual == Fase21::EscolhendoAcoes;

        estado["banca"]["pontuacao"] =
            esconderSegundaCarta ? 0 : banca->calcularPontuacao();

        estado["banca"]["estourou"] =
            esconderSegundaCarta ? false : banca->estourou();

        estado["banca"]["mao"] =
            maoParaJson(banca->verMao(), esconderSegundaCarta);
    }

    estado["pode_pedir"] = iniciou && jogo_.podeAgir(idLogicoJogo(idJogador));
    estado["pode_parar"] = iniciou && jogo_.podeAgir(idLogicoJogo(idJogador));
    estado["pode_iniciar_nova_rodada"] =
        iniciou && faseAtual == Fase21::Resultado && idJogador == 0;

    return estado;
}

Jogo21& SalaBlackJack::jogo() {
    return jogo_;
}

const Jogo21& SalaBlackJack::jogo() const {
    return jogo_;
}

std::vector<ConexaoBlackJack>& SalaBlackJack::conexoes() {
    return conexoes_;
}

const std::vector<ConexaoBlackJack>& SalaBlackJack::conexoes() const {
    return conexoes_;
}
