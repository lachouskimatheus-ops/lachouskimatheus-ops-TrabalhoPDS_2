#include <iostream>
#include "Baralho.hpp"
#include "Jogador.hpp"

int main() {

	Baralho baralho;
	baralho.embaralhar();

	Jogador jogador1("João Luís");

	for (int i = 0; i < 3; i++) {
		jogador1.receberCarta(baralho.puxarCarta());
	};

	jogador1.mostrarmao();


	return 0;
};