#include "coreAPI/SalaBase.hpp"

SalaBase::SalaBase(const std::string& idSala, int maxJogadores)
    : idSala_(idSala),
      maxJogadores_(maxJogadores),
      jogadoresConectados_(0) {
}

int SalaBase::adicionarJogador() {
    if (estaCheia()) {
        return -1;
    }

    for (JogadorConectado& jogador : jogadores_) {
        if (!jogador.conectado) {
            jogador.conectado = true;
            jogadoresConectados_++;
            return jogador.idJogador;
        }
    }

    int idJogador = static_cast<int>(jogadores_.size());

    if (idJogador >= maxJogadores_) {
        return -1;
    }

    jogadores_.push_back({idJogador, true});
    jogadoresConectados_++;

    return idJogador;
}

bool SalaBase::removerJogador(int idJogador) {
    for (JogadorConectado& jogador : jogadores_) {
        if (jogador.idJogador != idJogador) {
            continue;
        }

        if (!jogador.conectado) {
            return false;
        }

        jogador.conectado = false;
        jogadoresConectados_--;

        return true;
    }

    return false;
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

bool SalaBase::estaCheia() const {
    return jogadoresConectados_ >= maxJogadores_;
}

bool SalaBase::estaVazia() const {
    return jogadoresConectados_ == 0;
}

const std::vector<JogadorConectado>& SalaBase::jogadores() const {
    return jogadores_;
}