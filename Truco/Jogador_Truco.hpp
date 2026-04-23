#pragma once
#include <iostream>
#include "Carta.hpp"
#include <vector>
#include <string>

class Jogador {

private:
	std::string nome;
	std::vector<Carta> mao;

public:
	Jogador(std::string nome);
	void receberCarta(Carta novacarta);
	void mostrarmao();
};