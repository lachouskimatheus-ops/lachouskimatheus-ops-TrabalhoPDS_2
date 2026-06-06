#pragma once
#include <iostream>
#include <string>
#include "json.hpp"

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
    nlohmann::json paraJson() const;

};