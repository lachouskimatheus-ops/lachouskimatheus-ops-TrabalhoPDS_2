#pragma once
#include <vector>
#include "Jogador.hpp"
#include "JogadorFDP.hpp"

class Placar {
public:
	Placar() = default; //Construtor e destrutor padrões
	~Placar() = default;

	void exibirPlacar(std::vector<Jogador*> jogadores);
	void calcularResultados(std::vector<Jogador*> jogadores);
	int jogadoresVivos(std::vector<Jogador*> jogadores);
};