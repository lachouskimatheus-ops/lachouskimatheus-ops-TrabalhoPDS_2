#include "Carta.hpp"

Carta::Carta(int valor, Naipe naipe){
	this->valor = valor;
	this->naipe = naipe;
}

void Carta::imprimir() {
	std::cout << "\nValor: " << this->valor << " " << "Naipe: " << static_cast<int>(this->naipe) << std::endl;
}