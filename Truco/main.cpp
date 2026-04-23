#include <iostream>
#include "Baralho.hpp"
#include "Jogador.hpp"
#include "Menu.hpp"

void iniciar_truco() {
	std::cout << "teste\n";

	Baralho baralho;
	baralho.embaralhar();

	Jogador jogador1("João Luís");

	for (int i = 0; i < 3; i++) {
		jogador1.receberCarta(baralho.puxarCarta());
	};

	jogador1.mostrarmao();

	std::cout << "\n(Aperte Enter para voltar ao menu)";
	std::cin.ignore();
	std::cin.get();


};

int main() {

	Menu menu;
	menu.addOpcao("Truco", iniciar_truco);
	while (true) {
		menu.exibir();
	}


	return 0;
};