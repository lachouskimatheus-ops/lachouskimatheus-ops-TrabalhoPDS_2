#include "multiplayer/Pife/SalaPife.hpp"

SalaPife::SalaPife(
    const std::string& idSala,
    int maxJogadores
)
    : SalaBase(idSala, maxJogadores),
      jogo_(maxJogadores),
      partidaIniciada_(false) {
}

ConexaoPife* SalaPife::buscarConexaoDoJogador(
    int idJogador
) {
    for (ConexaoPife& registro : conexoes_) {
        if (registro.idJogador == idJogador) {
            return &registro;
        }
    }

    return nullptr;
}

const ConexaoPife*
SalaPife::buscarConexaoDoJogador(
    int idJogador
) const {
    for (
        const ConexaoPife& registro :
        conexoes_
    ) {
        if (registro.idJogador == idJogador) {
            return &registro;
        }
    }

    return nullptr;
}

int SalaPife::adicionarJogador(
    crow::websocket::connection* conexao,
    const std::string& tokenReconexao
) {
    if (
        conexao == nullptr
        || tokenReconexao.empty()
    ) {
        return -1;
    }

    int idConexaoExistente =
        obterIdJogador(conexao);

    if (idConexaoExistente != -1) {
        return idConexaoExistente;
    }

    if (tokenExiste(tokenReconexao)) {
        return reconectarJogador(
            conexao,
            tokenReconexao
        );
    }

    if (!podeReceberNovoJogador()) {
        return -1;
    }

    int idJogador =
        SalaBase::adicionarJogador(
            tokenReconexao
        );

    if (idJogador == -1) {
        return -1;
    }

    conexoes_.push_back({
        idJogador,
        conexao
    });

    if (todosConectados()) {
        iniciarPartida();
    }

    return idJogador;
}

int SalaPife::reconectarJogador(
    crow::websocket::connection* conexao,
    const std::string& tokenReconexao
) {
    if (
        conexao == nullptr
        || tokenReconexao.empty()
    ) {
        return -1;
    }

    int idJogador =
        SalaBase::reconectarJogador(
            tokenReconexao
        );

    if (idJogador == -1) {
        return -1;
    }

    ConexaoPife* registro =
        buscarConexaoDoJogador(idJogador);

    if (registro != nullptr) {
        registro->conexao = conexao;
    }
    else {
        conexoes_.push_back({
            idJogador,
            conexao
        });
    }

    return idJogador;
}

bool SalaPife::removerConexao(
    crow::websocket::connection* conexao
) {
    if (conexao == nullptr) {
        return false;
    }

    for (
        auto it = conexoes_.begin();
        it != conexoes_.end();
        ++it
    ) {
        if (it->conexao != conexao) {
            continue;
        }

        int idJogador = it->idJogador;

        conexoes_.erase(it);

        return SalaBase::desconectarJogador(
            idJogador
        );
    }

    return false;
}

int SalaPife::obterIdJogador(
    crow::websocket::connection* conexao
) const {
    if (conexao == nullptr) {
        return -1;
    }

    for (
        const ConexaoPife& registro :
        conexoes_
    ) {
        if (registro.conexao == conexao) {
            return registro.idJogador;
        }
    }

    return -1;
}

crow::websocket::connection*
SalaPife::obterConexaoJogador(
    int idJogador
) const {
    const ConexaoPife* registro =
        buscarConexaoDoJogador(idJogador);

    if (registro == nullptr) {
        return nullptr;
    }

    return registro->conexao;
}

bool SalaPife::possuiConexao(
    crow::websocket::connection* conexao
) const {
    return obterIdJogador(conexao) != -1;
}

bool SalaPife::partidaIniciada() const {
    return partidaIniciada_;
}

bool SalaPife::podeReceberNovoJogador() const {
    if (partidaIniciada_) {
        return false;
    }

    return podeAdicionarNovoJogador();
}

bool SalaPife::podeReconectar(
    const std::string& tokenReconexao
) const {
    return tokenExiste(tokenReconexao);
}

void SalaPife::iniciarPartida() {
    if (partidaIniciada_) {
        return;
    }

    if (!todosConectados()) {
        return;
    }

    partidaIniciada_ = true;
}

Pife& SalaPife::jogo() {
    return jogo_;
}

const Pife& SalaPife::jogo() const {
    return jogo_;
}

std::vector<ConexaoPife>&
SalaPife::conexoes() {
    return conexoes_;
}

const std::vector<ConexaoPife>&
SalaPife::conexoes() const {
    return conexoes_;
}