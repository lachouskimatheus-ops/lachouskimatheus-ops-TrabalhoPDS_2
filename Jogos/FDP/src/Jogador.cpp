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

void Jogador::mostrarMao() const {
    for (size_t i = 0; i < mao_.size(); i++) {
        std::cout << "[" << i + 1 << "] "; // Mostra [1], [2], etc.
        mao_[i]->imprimir(); 
        std::cout << "\n";
    }
}

Carta* Jogador::jogarCarta(int posicao) {

	Carta* carta = mao_[posicao];
	mao_.erase(mao_.begin() + posicao);
	return carta;
}

int Jogador::getQtdCartasMao() const {
	return mao_.size();
}