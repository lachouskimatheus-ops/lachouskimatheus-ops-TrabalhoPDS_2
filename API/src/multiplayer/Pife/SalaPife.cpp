#include "multiplayer/Pife/SalaPife.hpp"

SalaPife::SalaPife(const std::string& idSala, int maxJogadores)
    : SalaBase(idSala, maxJogadores),
      jogo_(maxJogadores) {
}

int SalaPife::adicionarJogador(crow::websocket::connection* conn) {
    int idJogador = SalaBase::adicionarJogador();

    if (idJogador == -1) {
        return -1;
    }

    conexoes_.push_back({idJogador, conn});

    return idJogador;
}

Pife& SalaPife::jogo() {
    return jogo_;
}

std::vector<ConexaoPife>& SalaPife::conexoes() {
    return conexoes_;
}