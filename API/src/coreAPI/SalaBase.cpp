#include "coreAPI/SalaBase.hpp"

SalaBase::SalaBase(
    const std::string& idSala,
    int maxJogadores
)
    : idSala_(idSala),
      maxJogadores_(maxJogadores),
      jogadoresConectados_(0) {
}

int SalaBase::adicionarJogador(
    const std::string& tokenReconexao
) {
    if (tokenReconexao.empty()) {
        return -1;
    }

    int jogadorExistente =
        buscarJogadorPorToken(tokenReconexao);

    if (jogadorExistente != -1) {
        return reconectarJogador(tokenReconexao);
    }

    if (!podeAdicionarNovoJogador()) {
        return -1;
    }

    int idJogador =
        static_cast<int>(jogadores_.size());

    jogadores_.push_back({
        idJogador,
        true,
        tokenReconexao
    });

    jogadoresConectados_++;

    return idJogador;
}

int SalaBase::reconectarJogador(
    const std::string& tokenReconexao
) {
    if (tokenReconexao.empty()) {
        return -1;
    }

    for (JogadorConectado& jogador : jogadores_) {
        if (
            jogador.tokenReconexao
            != tokenReconexao
        ) {
            continue;
        }

        if (!jogador.conectado) {
            jogador.conectado = true;
            jogadoresConectados_++;
        }

        return jogador.idJogador;
    }

    return -1;
}

bool SalaBase::desconectarJogador(
    int idJogador
) {
    for (JogadorConectado& jogador : jogadores_) {
        if (jogador.idJogador != idJogador) {
            continue;
        }

        if (!jogador.conectado) {
            return false;
        }

        jogador.conectado = false;

        if (jogadoresConectados_ > 0) {
            jogadoresConectados_--;
        }

        return true;
    }

    return false;
}

int SalaBase::buscarJogadorPorToken(
    const std::string& tokenReconexao
) const {
    for (
        const JogadorConectado& jogador :
        jogadores_
    ) {
        if (
            jogador.tokenReconexao
            == tokenReconexao
        ) {
            return jogador.idJogador;
        }
    }

    return -1;
}

bool SalaBase::tokenExiste(
    const std::string& tokenReconexao
) const {
    return buscarJogadorPorToken(
        tokenReconexao
    ) != -1;
}

bool SalaBase::jogadorEstaConectado(
    int idJogador
) const {
    for (
        const JogadorConectado& jogador :
        jogadores_
    ) {
        if (jogador.idJogador == idJogador) {
            return jogador.conectado;
        }
    }

    return false;
}

bool SalaBase::podeAdicionarNovoJogador() const {
    return !estaLotada();
}

std::string SalaBase::idSala() const {
    return idSala_;
}

int SalaBase::maxJogadores() const {
    return maxJogadores_;
}

int SalaBase::jogadoresConectados() const {
    return jogadoresConectados_;
}

int SalaBase::jogadoresRegistrados() const {
    return static_cast<int>(
        jogadores_.size()
    );
}

bool SalaBase::estaLotada() const {
    return jogadoresRegistrados()
        >= maxJogadores_;
}

bool SalaBase::todosConectados() const {
    return estaLotada()
        && jogadoresConectados_
            >= maxJogadores_;
}

bool SalaBase::estaVazia() const {
    return jogadoresConectados_ == 0;
}

const std::vector<JogadorConectado>&
SalaBase::jogadores() const {
    return jogadores_;
}