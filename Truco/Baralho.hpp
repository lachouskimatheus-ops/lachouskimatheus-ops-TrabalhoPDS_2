#pragma once
#include <iostream>
#include <vector>
#include "Carta.hpp"

class Baralho {

private:
	std::vector<Carta> monte;

public:
	Baralho();
	void embaralhar();
	Carta puxarCarta();

};