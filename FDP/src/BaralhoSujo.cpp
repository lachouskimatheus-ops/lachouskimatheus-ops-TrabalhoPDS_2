#include "Carta.hpp"
#include "BaralhoSujo.hpp"

BaralhoSujo::BaralhoSujo() {
	inicializar();
}

void BaralhoSujo::inicializar(){
	Naipe naipes[] = {Naipe::paus, Naipe::copas, Naipe:: espadas, Naipe:: ouros};

	for (Naipe naipe : naipes) {
		for(int i = 1; i <= 13; i++) {
			if(i == 8 || i == 9 || i == 10) continue;
			Carta* novaCarta = new Carta(i, naipe);
			cartas_.push_back(novaCarta);
		};
	};
}