#include "Baralho.hpp"
#include <random>
#include <algorithm>

Baralho::Baralho() {
	for (int i = 0; i < 4; i++){
		for (int j = 1; j <= 13; j++) {
			if (j == 8 || j == 9 || j == 10) continue;

			Naipe naipeAtual = static_cast<Naipe>(i);

			Carta novacarta(j, naipeAtual);
			this->monte.push_back(novacarta);

		}
	}
}

void Baralho::embaralhar() {

	std::random_device rd;
	std::mt19937 gerador(rd());

	std::shuffle(monte.begin(), monte.end(), gerador);
}

Carta Baralho::puxarCarta() {

	if (monte.empty()) throw std::runtime_error("Acabou as cartas, panaca!");

	Carta topo = monte.back();
	monte.pop_back();
	return topo;
}