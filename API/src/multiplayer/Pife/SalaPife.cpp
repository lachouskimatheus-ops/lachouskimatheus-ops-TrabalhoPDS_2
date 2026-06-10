#include "multiplayer/Pife/SalaPife.hpp"

SalaPife::SalaPife(const std::string& idSala, int maxJogadores)
    : SalaBase(idSala, maxJogadores), jogo_(maxJogadores), partidaIniciada_(false) {}

ConexaoPife* SalaPife::buscarConexaoDoJogador(int idJogador) {
    for (auto& registro : conexoes_)
        if (registro.idJogador == idJogador) return &registro;

    return nullptr;
}

const ConexaoPife* SalaPife::buscarConexaoDoJogador(int idJogador) const {
    for (const auto& registro : conexoes_)
        if (registro.idJogador == idJogador) return &registro;

    return nullptr;
}

int SalaPife::adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome) {
    if (!conexao || tokenReconexao.empty()) return -1;

    int idExistente = obterIdJogador(conexao);
    if (idExistente != -1) return idExistente;

    if (tokenExiste(tokenReconexao))
        return reconectarJogador(conexao, tokenReconexao);

    if (!podeReceberNovoJogador()) return -1;

    int idJogador = SalaBase::adicionarJogador(tokenReconexao);
    if (idJogador == -1) return -1;

    conexoes_.push_back({idJogador, conexao});

    std::string nomeFinal = nome;
    if (nomeFinal.empty()) nomeFinal = "Jogador " + std::to_string(idJogador + 1);
    if (nomeFinal.size() > 20) nomeFinal.resize(20);

    jogo_.consultarJogador(idJogador).definirNome(nomeFinal);

    if (todosConectados()) iniciarPartida();

    return idJogador;
}

int SalaPife::reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao) {
    if (!conexao || tokenReconexao.empty()) return -1;

    int idJogador = SalaBase::reconectarJogador(tokenReconexao);
    if (idJogador == -1) return -1;

    ConexaoPife* registro = buscarConexaoDoJogador(idJogador);

    if (registro) registro->conexao = conexao;
    else conexoes_.push_back({idJogador, conexao});

    return idJogador;
}

bool SalaPife::removerConexao(crow::websocket::connection* conexao) {
    if (!conexao) return false;

    for (auto it = conexoes_.begin(); it != conexoes_.end(); ++it) {
        if (it->conexao != conexao) continue;

        int idJogador = it->idJogador;
        conexoes_.erase(it);

        return SalaBase::desconectarJogador(idJogador);
    }

    return false;
}

int SalaPife::obterIdJogador(crow::websocket::connection* conexao) const {
    if (!conexao) return -1;

    for (const auto& registro : conexoes_)
        if (registro.conexao == conexao) return registro.idJogador;

    return -1;
}

crow::websocket::connection* SalaPife::obterConexaoJogador(int idJogador) const {
    const ConexaoPife* registro = buscarConexaoDoJogador(idJogador);
    return registro ? registro->conexao : nullptr;
}

bool SalaPife::possuiConexao(crow::websocket::connection* conexao) const {
    return obterIdJogador(conexao) != -1;
}

bool SalaPife::partidaIniciada() const {
    return partidaIniciada_;
}

bool SalaPife::podeReceberNovoJogador() const {
    return !partidaIniciada_ && podeAdicionarNovoJogador();
}

bool SalaPife::podeReconectar(const std::string& tokenReconexao) const {
    return tokenExiste(tokenReconexao);
}

void SalaPife::iniciarPartida() {
    if (partidaIniciada_ || !todosConectados()) return;
    partidaIniciada_ = true;
}

Pife& SalaPife::jogo() {
    return jogo_;
}

const Pife& SalaPife::jogo() const {
    return jogo_;
}

std::vector<ConexaoPife>& SalaPife::conexoes() {
    return conexoes_;
}

const std::vector<ConexaoPife>& SalaPife::conexoes() const {
    return conexoes_;
}