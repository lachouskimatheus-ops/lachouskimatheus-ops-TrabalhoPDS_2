#include "Baralho.hpp"
#include "Carta.hpp"
#include <random>
#include <algorithm>

void Baralho::embaralhar() {

	std::random_device rd;
	std::mt19937 gerador(rd());

	std::shuffle(cartas_.begin(), cartas_.end(), gerador);
}

Baralho::~Baralho() {
	for(int i = 0; i < cartas_.size(); i++) {
		delete cartas_[i];
	};
}

Carta* Baralho::puxarCarta() {

	if (cartas_.empty()) throw std::runtime_error("Sem cartas no baralho!");

	Carta* topo = cartas_.back();
	cartas_.pop_back();
	return topo;
}

int Baralho::getQtdCartas() const {
	return cartas_.size();
}