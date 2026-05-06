#pragma once
#include <iostream>

enum class Naipe {
	paus,
	copas,
	espadas,
	ouros,
};

class Carta {

private:
	int valor;
	Naipe naipe;

public:
	Carta(int valor, Naipe naipe);
	void imprimir();

};