#include "Jogador.hpp"
#include <iostream>
#include "Carta.hpp"

Jogador::Jogador(int id, std::string nome) {
	nome_ = nome;
	id_ = id;
}

std::string Jogador::getNome() const {
	return nome_;
}

int Jogador::getId() const {
	return id_;
}

void Jogador::receberCarta(Carta* novaCarta) {
	mao_.push_back(novaCarta);
}

Jogador::~Jogador() {
}

Carta* Jogador::jogarCarta(int posicao) {
    // Garante que a posição não é negativa e não passa do tamanho do vetor
    if (posicao >= 0 && posicao < mao_.size()) {
        Carta* carta = mao_[posicao];
        mao_.erase(mao_.begin() + posicao);
        return carta;
    }
    return nullptr;
}

int Jogador::getQtdCartasMao() const {
	return mao_.size();
}

void Jogador::setNome(std::string novoNome) {
    nome_ = novoNome;
}