#include "Carta.hpp"
#include "BaralhoSujo.hpp"

BaralhoSujo::BaralhoSujo() {
	inicializar();
}

void BaralhoSujo::inicializar(){
	std::string naipes[4] = {"Paus", "Copas", "Espadas", "Ouros"};

	for (const std::string& naipe : naipes) {
		for(int i = 1; i <= 13; i++) {
			if(i == 8 || i == 9 || i == 10) continue;

			cartas_.push_back(Carta(i, naipe));
		};
	};
}