#include "multiplayer/Pife/SalaPife.hpp"

SalaPife::SalaPife(const std::string& idSala, int maxJogadores)
    : SalaBase(idSala, maxJogadores),
      jogo_(maxJogadores),
      partidaIniciada_(false) {
}

int SalaPife::adicionarJogador(crow::websocket::connection* conexao) {
    if (conexao == nullptr) {
        return -1;
    }

    if (!podeReceberJogador()) {
        return -1;
    }

    for (const ConexaoPife& conexaoRegistrada : conexoes_) {
        if (conexaoRegistrada.conexao == conexao) {
            return conexaoRegistrada.idJogador;
        }
    }

    int idJogador = SalaBase::adicionarJogador();

    if (idJogador == -1) {
        return -1;
    }

    conexoes_.push_back({idJogador, conexao});

    if (estaCheia()) {
        iniciarPartida();
    }

    return idJogador;
}

bool SalaPife::removerConexao(crow::websocket::connection* conexao) {
    for (auto it = conexoes_.begin(); it != conexoes_.end(); ++it) {
        if (it->conexao != conexao) {
            continue;
        }

        int idJogador = it->idJogador;

        conexoes_.erase(it);

        return SalaBase::removerJogador(idJogador);
    }

    return false;
}

int SalaPife::obterIdJogador(crow::websocket::connection* conexao) const {
    for (const ConexaoPife& conexaoRegistrada : conexoes_) {
        if (conexaoRegistrada.conexao == conexao) {
            return conexaoRegistrada.idJogador;
        }
    }

    return -1;
}

bool SalaPife::estaCheia() const {
    return SalaBase::estaCheia();
}

bool SalaPife::partidaIniciada() const {
    return partidaIniciada_;
}

bool SalaPife::podeReceberJogador() const {
    if (partidaIniciada_) {
        return false;
    }

    return !estaCheia();
}

void SalaPife::iniciarPartida() {
    if (partidaIniciada_) {
        return;
    }

    if (!estaCheia()) {
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

std::vector<ConexaoPife>& SalaPife::conexoes() {
    return conexoes_;
}

const std::vector<ConexaoPife>& SalaPife::conexoes() const {
    return conexoes_;
}