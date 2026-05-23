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
	int valor_;
	Naipe naipe_;

public:
	Carta() = default;
	Carta(int valor, Naipe naipe);
	std::string toString() const;

	int getValor() const;
	Naipe getNaipe() const;

	int forca() const; 
    int getForcaNaipe() const;

};