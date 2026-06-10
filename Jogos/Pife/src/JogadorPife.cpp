#include "JogadorPife.hpp"
#include <algorithm>

JogadorPife::JogadorPife() = default;

bool JogadorPife::indiceValido(int indice) const {
    return (
        indice >= 0 &&
        indice < static_cast<int>(mao_.size())
    );
}

void JogadorPife::receberCarta(const Carta& carta) {
    mao_.push_back(carta);
}

#include <algorithm>

void JogadorPife::organizarMao() {
    std::sort(mao_.begin(), mao_.end(),
        [](const Carta& carta1, const Carta& carta2) {

            if (carta1.getValor() == carta2.getValor()) {
                return carta1.getNaipe() < carta2.getNaipe();
            }

            return carta1.getValor() < carta2.getValor();
        }
    );
}


int JogadorPife::tmnhMao() const {
    return static_cast<int>(
        mao_.size()
    );
}

Carta JogadorPife::descartarCarta(int indice) {
    if (!indiceValido(indice)) {
        return Carta();
    }

    Carta cartaDescartada = mao_[indice];
    mao_.erase(
        mao_.begin() + indice
    );

    return cartaDescartada;
}

const std::vector<Carta>& JogadorPife::verMao() const {
    return mao_;
}

void JogadorPife::definirNome(const std::string& nome) {
    nome_ = nome;
}

const std::string& JogadorPife::nome() const {
    return nome_;
}