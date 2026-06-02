#include "jogadorPife.h"

JogadorPife::JogadorPife() {
    podeComprarMesa_ = true;
}

void JogadorPife::receberCarta(const Carta& carta) {
    mao_.push_back(carta);
}

int JogadorPife::tmnhMao() const {
    return mao_.size();
}

void JogadorPife::organizarMao() {
    for (int i = 0; i < static_cast<int>(mao_.size()) - 1; i++) {
        for (int j = 0; j < static_cast<int>(mao_.size()) - 1 - i; j++) {
            int valorAtual = static_cast<int>(mao_[j].mostraValor());
            int valorProximo = static_cast<int>(mao_[j + 1].mostraValor());

            if (valorAtual > valorProximo) {
                Carta temp = mao_[j];
                mao_[j] = mao_[j + 1];
                mao_[j + 1] = temp;
            }
        }
    }
}

void JogadorPife::bloquearCompraMesa() {
    podeComprarMesa_ = false;
}

void JogadorPife::liberarCompraMesa() {
    podeComprarMesa_ = true;
}

bool JogadorPife::podeComprarMesa() const {
    return podeComprarMesa_;
}

Carta JogadorPife::descartarCarta(int indice) {
    if (indice < 0 || indice >= static_cast<int>(mao_.size())) {
        return Carta();
    }

    Carta descartada = mao_[indice];
    mao_.erase(mao_.begin() + indice);

    return descartada;
}

const std::vector<Carta>& JogadorPife::verMao() const {
    return mao_;
}