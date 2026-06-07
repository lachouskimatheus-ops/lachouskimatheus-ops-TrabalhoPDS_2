#include "Pife.hpp"
#include <stdexcept>

Pife::Pife(int quantidadeJogadores)
    : baralho_(2),
      jogadorAtual_(0),
      vencedor_(-1),
      jogoFinalizado_(false),
      faseTurno_(FaseTurno::AguardandoCompra) {

    jogadores_.reserve(quantidadeJogadores);

    for (int i = 0; i < quantidadeJogadores; i++) {
        jogadores_.push_back(JogadorPife());
    }

    baralho_.embaralhar();
    distribuirCartas();
}

void Pife::distribuirCartas() {
    for (int rodada = 0; rodada < 9; rodada++) {
        for (int idJogador = 0; idJogador < static_cast<int>(jogadores_.size()); idJogador++) {
            if (baralho_.estaVazio()) {
                throw std::runtime_error(
                    "O baralho não possui cartas suficientes."
                );
            }

            jogadores_[idJogador].receberCarta(
                baralho_.retirarCarta()
            );
        }
    }

    if (baralho_.estaVazio()) {
        throw std::runtime_error(
            "Não há carta disponível para definir a vira."
        );
    }
    vira_ = baralho_.retirarCarta();

    mesa_.clear();

    jogadorAtual_ = 0;
    vencedor_ = -1;
    jogoFinalizado_ = false;
    faseTurno_ = FaseTurno::AguardandoCompra;
}

bool Pife::jogadorValido(int idJogador) const {
    return idJogador >= 0 && idJogador < static_cast<int>(jogadores_.size());
}

bool Pife::podeComprarBaralho(int idJogador) const {
    if (jogoFinalizado_) {
        return false;
    }

    if (!jogadorValido(idJogador)) {
        return false;
    }

    if (idJogador != jogadorAtual_) {
        return false;
    }

    if (faseTurno_ != FaseTurno::AguardandoCompra) {
        return false;
    }

    if (!baralho_.estaVazio()) {
        return true;
    }

    return mesa_.size() > 1;
}

bool Pife::comprarMesa(int idJogador) {
    if (!podeComprarMesa(idJogador)) {
        return false;
    }
    Carta cartaComprada = mesa_.back();
    mesa_.pop_back();
    jogadores_[idJogador].receberCarta(
        cartaComprada
    );
    faseTurno_ = FaseTurno::AguardandoDescarte;
    return true;
}

bool Pife::colocarNaMesa(int idJogador, int indiceCarta) {
    if (!podeColocarNaMesa(idJogador)) {
        return false;
    }

    if (indiceCarta < 0 || indiceCarta >= jogadores_[idJogador].tmnhMao()) {
        return false;
    }

    Carta cartaDescartada = jogadores_[idJogador].descartarCarta(indiceCarta);

    if (!cartaDescartada.validacaoCarta()) {
        return false;
    }
    mesa_.push_back(cartaDescartada);
    proximoJogador();
    faseTurno_ = FaseTurno::AguardandoCompra;
    return true;
}

bool Pife::organizarMao(int idJogador) {
    if (!jogadorValido(idJogador)) {
        return false;
    }

    if (jogoFinalizado_) {
        return false;
    }

    jogadores_[idJogador].organizarMao();

    return true;
}

bool Pife::bati(int idJogador) {
    if (!podeBater(idJogador)) {
        return false;
    }

    const std::vector<Carta>& mao =
        jogadores_[idJogador].verMao();

    if (!verificaVitoria(mao, vira_)) {
        return false;
    }

    jogoFinalizado_ = true;
    vencedor_ = idJogador;
    faseTurno_ = FaseTurno::Finalizado;

    return true;
}

void Pife::proximoJogador() {
    if (jogadores_.empty()) {
        return;
    }
    jogadorAtual_++;

    if (jogadorAtual_ >= static_cast<int>(jogadores_.size())) {
        jogadorAtual_ = 0;
    }
}

bool Pife::comprarBaralho(int idJogador) {
    if (!podeComprarBaralho(idJogador)) {
        return false;
    }

    if (baralho_.estaVazio()) {
        reporBaralhoComDescarte();
    }

    if (baralho_.estaVazio()) {
        return false;
    }

    jogadores_[idJogador].receberCarta(
        baralho_.retirarCarta()
    );

    faseTurno_ = FaseTurno::AguardandoDescarte;

    return true;
}

bool Pife::podeComprarMesa(int idJogador) const {
    if (jogoFinalizado_) {
        return false;
    }

    if (!jogadorValido(idJogador)) {
        return false;
    }

    if (idJogador != jogadorAtual_) {
        return false;
    }

    if (faseTurno_ != FaseTurno::AguardandoCompra) {
        return false;
    }

    if (mesa_.empty()) {
        return false;
    }

    return true;
}

bool Pife::podeColocarNaMesa(int idJogador) const {
    if (jogoFinalizado_) {
        return false;
    }

    if (!jogadorValido(idJogador)) {
        return false;
    }

    if (idJogador != jogadorAtual_) {
        return false;
    }

    if (faseTurno_ != FaseTurno::AguardandoDescarte) {
        return false;
    }

    return true;
}

bool Pife::podeBater(int idJogador) const {
    if (jogoFinalizado_) {
        return false;
    }

    if (!jogadorValido(idJogador)) {
        return false;
    }

    if (idJogador != jogadorAtual_) {
        return false;
    }

    const int quantidadeCartas =
        jogadores_[idJogador].tmnhMao();


//Pode bater antes de comprar, com 9 cartas.
    if (faseTurno_ == FaseTurno::AguardandoCompra && quantidadeCartas == 9) {
        return true;
    }

//Pode bater depois de comprar, com 10 cartas.
    if (faseTurno_ == FaseTurno::AguardandoDescarte && quantidadeCartas == 10) {
        return true;
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
    return static_cast<int>(
        jogadores_.size()
    );
}

int Pife::quantidadeCartasBaralho() const {
    return baralho_.tamanho();
}

JogadorPife& Pife::consultarJogador(int idJogador) {
    if (!jogadorValido(idJogador)) {
        throw std::out_of_range(
            "ID de jogador inválido."
        );
    }

    return jogadores_[idJogador];
}


const JogadorPife& Pife::consultarJogador(int idJogador) const {
    if (!jogadorValido(idJogador)) {
        throw std::out_of_range(
            "ID de jogador inválido."
        );
    }

    return jogadores_[idJogador];
}

const std::vector<Carta>& Pife::consultarMao(int idJogador) const {
    if (!jogadorValido(idJogador)) {
        throw std::out_of_range(
            "ID de jogador inválido."
        );
    }
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
//Só é possível reconstruir o monte quando existem pelo menos duas cartas na mesa:
    if (!baralho_.estaVazio()) {
        return;
    }

    if (mesa_.size() <= 1) {
        return;
    }

    Carta cartaDoTopo = mesa_.back();

    mesa_.pop_back();

    for (const Carta& carta : mesa_) {
        baralho_.adicionarCarta(carta);
    }

    mesa_.clear();

//A última carta descartada continua sendo a carta disponível
    mesa_.push_back(cartaDoTopo);

    baralho_.embaralhar();
}