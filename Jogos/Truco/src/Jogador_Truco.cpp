#include "Jogador_Truco.hpp"

Jogador_Truco::Jogador_Truco(const std::string& nome) : nome_(nome) {}

void Jogador_Truco::receberCarta(Carta* novaCarta) {
    if (novaCarta != nullptr) mao_.push_back(novaCarta);
}

void Jogador_Truco::limparMao() {
    mao_.clear();
}

Carta* Jogador_Truco::jogarCarta(int indice) {
    if (indice < 0 || indice >= static_cast<int>(mao_.size())) return nullptr;

    Carta* carta = mao_[indice];
    mao_.erase(mao_.begin() + indice);
    return carta;
}

const std::string& Jogador_Truco::getNome() const {
    return nome_;
}

const std::vector<Carta*>& Jogador_Truco::getMao() const {
    return mao_;
}

int Jogador_Truco::getQuantidadeCartas() const {
    return static_cast<int>(mao_.size());
}