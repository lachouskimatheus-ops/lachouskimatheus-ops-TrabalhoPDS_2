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

	Carta* carta = mao_[posicao];
	mao_.erase(mao_.begin() + posicao);
	return carta;
}

int Jogador::getQtdCartasMao() const {
	return mao_.size();
}