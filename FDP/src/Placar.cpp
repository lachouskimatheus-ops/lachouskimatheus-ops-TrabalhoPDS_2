#include "Placar.hpp"
#include <iostream>

void Placar::exibirPlacar(std::vector<Jogador*> jogadores) {
	std::cout << "\n\n---> Placar Atual <---\n";
	for(auto j : jogadores) {
		JogadorFDP* jogador = static_cast<JogadorFDP*>(j);

		std::cout << "Jogador: " << jogador->getNome() << " | Vidas: ";
		if(jogador->getVidas() > 0) {
			std::cout << jogador->getVidas();
		} else {
			std::cout << "ELIMINADO!";
		};
		std::cout << std::endl;
	};
	std::cout << "---------------------";
}

int Placar::jogadoresVivos(std::vector<Jogador*> jogadores) {
	int vivos = 0;
	for(auto j : jogadores) {
		JogadorFDP* jogador = static_cast<JogadorFDP*>(j);
		if (jogador->getVidas() > 0) {
			vivos++;
		};
	};
	return vivos;
}

void Placar::calcularResultados(std::vector<Jogador*> jogadores){
	for(auto j : jogadores) {
		JogadorFDP* jogador = static_cast<JogadorFDP*>(j);

		if(jogador->getVidas() <= 0) continue;
		jogador->dano();
	};
}