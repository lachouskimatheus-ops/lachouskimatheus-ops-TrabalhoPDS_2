#include "Pife.hpp"

Pife::Pife(int quantidadeJogadores)
    : baralho_(2),
      jogadorAtual_(0),
      vencedor_(-1),
      jogoFinalizado_(false),
      faseTurno_(FaseTurno::AguardandoCompra) {

    if (quantidadeJogadores < 2 || quantidadeJogadores > 4) {
        throw QuantidadeJogadoresInvalida();
    }

    jogadores_.reserve(quantidadeJogadores);

    for (int i = 0; i < quantidadeJogadores; i++) {
        jogadores_.push_back(JogadorPife());
    }

    baralho_.embaralhar();
    distribuirCartas();
}

void Pife::distribuirCartas() {
    const int cartasNecessarias =
        static_cast<int>(jogadores_.size()) * 9 + 1;

    if (baralho_.tamanho() < cartasNecessarias) {
        throw BaralhoIndisponivel();
    }

    for (int rodada = 0; rodada < 9; rodada++) {
        for (JogadorPife& jogador : jogadores_) {
            jogador.receberCarta(baralho_.retirarCarta());
        }
    }

    vira_ = baralho_.retirarCarta();
    mesa_.clear();

    jogadorAtual_ = 0;
    vencedor_ = -1;
    jogoFinalizado_ = false;
    faseTurno_ = FaseTurno::AguardandoCompra;
}

bool Pife::jogadorValido(int idJogador) const {
    return idJogador >= 0 &&
           idJogador < static_cast<int>(jogadores_.size());
}

void Pife::validarJogador(int idJogador) const {
    if (!jogadorValido(idJogador)) {
        throw JogadorInvalido();
    }
}

void Pife::validarPartidaAtiva() const {
    if (jogoFinalizado_) {
        throw JogoFinalizado();
    }
}

void Pife::validarTurno(int idJogador) const {
    validarJogador(idJogador);
    validarPartidaAtiva();

    if (idJogador != jogadorAtual_) {
        throw ForaDoTurno();
    }
}

void Pife::comprarBaralho(int idJogador) {
    validarTurno(idJogador);

    if (faseTurno_ != FaseTurno::AguardandoCompra) {
        throw FaseTurnoInvalida(
            "O jogador já comprou uma carta e deve descartar."
        );
    }

    if (baralho_.estaVazio()) {
        reporBaralhoComDescarte();
    }

    if (baralho_.estaVazio()) {
        throw BaralhoIndisponivel();
    }

    jogadores_[idJogador].receberCarta(
        baralho_.retirarCarta()
    );

    faseTurno_ = FaseTurno::AguardandoDescarte;
}

void Pife::comprarMesa(int idJogador) {
    validarTurno(idJogador);

    if (faseTurno_ != FaseTurno::AguardandoCompra) {
        throw FaseTurnoInvalida(
            "O jogador já comprou uma carta e deve descartar."
        );
    }

    if (mesa_.empty()) {
        throw MesaVazia();
    }

    Carta cartaComprada = mesa_.back();
    mesa_.pop_back();

    jogadores_[idJogador].receberCarta(cartaComprada);
    faseTurno_ = FaseTurno::AguardandoDescarte;
}

void Pife::colocarNaMesa(int idJogador, int indiceCarta) {
    validarTurno(idJogador);

    if (faseTurno_ != FaseTurno::AguardandoDescarte) {
        throw FaseTurnoInvalida(
            "O jogador deve comprar uma carta antes de descartar."
        );
    }

    if (indiceCarta < 0 ||
        indiceCarta >= jogadores_[idJogador].tmnhMao()) {
        throw IndiceCartaInvalido();
    }

    Carta cartaDescartada =
        jogadores_[idJogador].descartarCarta(indiceCarta);

    if (!cartaDescartada.validacaoCarta()) {
        throw CartaInvalida();
    }

    mesa_.push_back(cartaDescartada);

    proximoJogador();
    faseTurno_ = FaseTurno::AguardandoCompra;
}

void Pife::organizarMao(int idJogador) {
    validarJogador(idJogador);
    validarPartidaAtiva();

    jogadores_[idJogador].organizarMao();
}

void Pife::bati(int idJogador) {
    validarTurno(idJogador);

    const int quantidadeCartas =
        jogadores_[idJogador].tmnhMao();

    const bool podeBaterAntesDaCompra =
        faseTurno_ == FaseTurno::AguardandoCompra &&
        quantidadeCartas == 9;

    const bool podeBaterDepoisDaCompra =
        faseTurno_ == FaseTurno::AguardandoDescarte &&
        quantidadeCartas == 10;

    if (!podeBaterAntesDaCompra &&
        !podeBaterDepoisDaCompra) {
        throw FaseTurnoInvalida(
            "O jogador não pode bater neste momento."
        );
    }

    const std::vector<Carta>& mao =
        jogadores_[idJogador].verMao();

    if (!verificaVitoria(mao, vira_)) {
        throw MaoInvalida();
    }

    jogoFinalizado_ = true;
    vencedor_ = idJogador;
    faseTurno_ = FaseTurno::Finalizado;
}

void Pife::proximoJogador() {
    if (jogadores_.empty()) {
        return;
    }

    jogadorAtual_ =
        (jogadorAtual_ + 1) %
        static_cast<int>(jogadores_.size());
}

bool Pife::podeComprarBaralho(int idJogador) const {
    if (jogoFinalizado_ ||
        !jogadorValido(idJogador) ||
        idJogador != jogadorAtual_ ||
        faseTurno_ != FaseTurno::AguardandoCompra) {
        return false;
    }

    return !baralho_.estaVazio() || mesa_.size() > 1;
}

bool Pife::podeComprarMesa(int idJogador) const {
    if (jogoFinalizado_ ||
        !jogadorValido(idJogador) ||
        idJogador != jogadorAtual_ ||
        faseTurno_ != FaseTurno::AguardandoCompra) {
        return false;
    }

    return !mesa_.empty();
}

bool Pife::podeColocarNaMesa(int idJogador) const {
    return !jogoFinalizado_ &&
           jogadorValido(idJogador) &&
           idJogador == jogadorAtual_ &&
           faseTurno_ == FaseTurno::AguardandoDescarte;
}

bool Pife::podeBater(int idJogador) const {
    if (jogoFinalizado_ ||
        !jogadorValido(idJogador) ||
        idJogador != jogadorAtual_) {
        return false;
    }

    const int quantidadeCartas =
        jogadores_[idJogador].tmnhMao();

    if (faseTurno_ == FaseTurno::AguardandoCompra) {
        return quantidadeCartas == 9;
    }

    if (faseTurno_ == FaseTurno::AguardandoDescarte) {
        return quantidadeCartas == 10;
    }

    return false;
}

bool Pife::jogoFinalizado() const {
    return jogoFinalizado_;
}

int Pife::consultarIndiceJogadorAtual() const {
    return jogadorAtual_;
}

int Pife::consultarVencedor() const {
    return vencedor_;
}

FaseTurno Pife::consultarFaseTurno() const {
    return faseTurno_;
}

int Pife::numeroDeJogadores() const {
    return static_cast<int>(jogadores_.size());
}

int Pife::quantidadeCartasBaralho() const {
    return baralho_.tamanho();
}

JogadorPife& Pife::consultarJogador(int idJogador) {
    validarJogador(idJogador);
    return jogadores_[idJogador];
}

const JogadorPife& Pife::consultarJogador(
    int idJogador
) const {
    validarJogador(idJogador);
    return jogadores_[idJogador];
}

const std::vector<Carta>& Pife::consultarMao(
    int idJogador
) const {
    validarJogador(idJogador);
    return jogadores_[idJogador].verMao();
}

const std::vector<Carta>& Pife::consultarMesa() const {
    return mesa_;
}

const Carta& Pife::consultarVira() const {
    return vira_;
}

bool Pife::cartaEhCoringa(const Carta& carta) const {
    return ::cartaEhCoringa(carta, vira_);
}

void Pife::reporBaralhoComDescarte() {
    if (!baralho_.estaVazio() || mesa_.size() <= 1) {
        return;
    }

    Carta cartaDoTopo = mesa_.back();
    mesa_.pop_back();

    for (const Carta& carta : mesa_) {
        baralho_.adicionarCarta(carta);
    }

    mesa_.clear();
    mesa_.push_back(cartaDoTopo);

    baralho_.embaralhar();
}