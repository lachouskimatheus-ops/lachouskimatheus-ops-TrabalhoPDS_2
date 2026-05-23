#pragma once
#include <iostream>
#include <string>

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
	Carta(int valor, std::string naipeS);
	void imprimir();
	int forca();

	int getValor() const;
	Naipe getNaipe() const;
	int getForcaNaipe() const;
};