#include "coreAPI/SalaBase.hpp"

SalaBase::SalaBase(const std::string& idSala, int maxJogadores)
    : idSala_(idSala),
      maxJogadores_(maxJogadores),
      jogadoresConectados_(0) {
}

int SalaBase::adicionarJogador() {
    if (jogadoresConectados_ >= maxJogadores_) {
        return -1;
    }

    int idJogador = jogadoresConectados_;

    jogadores_.push_back({idJogador, true});
    jogadoresConectados_++;

    return idJogador;
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

const std::vector<JogadorConectado>& SalaBase::jogadores() const {
    return jogadores_;
}