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

void JogadorPife::organizarMao() {std::sort(mao_.begin(), mao_.end(), [](const Carta& cartaA, const Carta& cartaB) {
            int naipeA = static_cast<int>(cartaA.mostraNaipe());
            int naipeB = static_cast<int>(cartaB.mostraNaipe());

            if (naipeA != naipeB) {
                return naipeA < naipeB;
            }
            return static_cast<int>(cartaA.mostraValor()) < static_cast<int>(cartaB.mostraValor()
            );
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