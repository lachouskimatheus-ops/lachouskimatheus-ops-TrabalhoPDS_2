#include "pife.h"

Pife::Pife(int quantidadeJogadores)
    : baralho_(2), jogadorAtual_(0), jogoFinalizado_(false) {
    for (int i = 0; i < quantidadeJogadores; i++) {
        jogadores_.push_back(JogadorPife());
    }

    baralho_.embaralhar();
    distribuirCartas();
}

void Pife::comprarBaralho() {
    if (baralho_.estaVazio()) {
        return;
    }

    jogadores_[jogadorAtual_].receberCarta(baralho_.retirarCarta());
}

void Pife::comprarMesa() {
    if (!jogadores_[jogadorAtual_].podeComprarMesa()) {
        return;
    }

    if (mesa_.empty()) {
        return;
    }

    Carta carta = mesa_.back();
    mesa_.pop_back();

    jogadores_[jogadorAtual_].receberCarta(carta);
}

void Pife::colocarNaMesa(int indice) {
    Carta descarte = jogadores_[jogadorAtual_].descartarCarta(indice);

    if (descarte.validacaoCarta()) {
        mesa_.push_back(descarte);
    }
}

void Pife::distribuirCartas() {
    for (int carta = 0; carta < 9; carta++) {
        for (int i = 0; i < static_cast<int>(jogadores_.size()); i++) {
            jogadorAtual_ = i;
            comprarBaralho();
        }
    }

    jogadorAtual_ = 0;
}

void Pife::proximoJogador() {
    jogadores_[jogadorAtual_].liberarCompraMesa();

    jogadorAtual_++;

    if (jogadorAtual_ >= static_cast<int>(jogadores_.size())) {
        jogadorAtual_ = 0;
    }
}

bool Pife::bati() {
    if (verificaVitoria(jogadores_[jogadorAtual_].verMao())) {
        jogoFinalizado_ = true;
        return true;
    }

    jogadores_[jogadorAtual_].bloquearCompraMesa();
    return false;
}

bool Pife::jogoFinalizado() const {
    return jogoFinalizado_;
}

JogadorPife& Pife::consultarJogador() {
    return jogadores_[jogadorAtual_];
}

const std::vector<Carta>& Pife::consultarMesa() const {
    return mesa_;
}

int Pife::consultarIndiceJogadorAtual() const {
    return jogadorAtual_;
}

int Pife::numeroDeJogadores() const {
    return jogadores_.size();
}