#include "Carta.hpp"

Carta::Carta(int valor, std::string naipeS){
	this->valor = valor;
	
	//Tradutor de strings		
	if (naipeS == "Paus") {this->naipe = Naipe::paus;}
	else if(naipeS == "Copas") {this->naipe = Naipe::copas;}
	else if(naipeS == "Espadas"){this->naipe = Naipe::espadas;}
	else {this->naipe = Naipe::ouros;}

}

void Carta::imprimir() {
	std::cout << "\nValor: " << this->valor << " " << "Naipe: " << static_cast<int>(this->naipe) << std::endl;
}

int Carta::forca(){
	switch(this->valor){
		case 4: return 1;
		case 5:  return 2;
        case 6:  return 3;
        case 7:  return 4;
        case 12: return 5; 
        case 11: return 6; 
        case 13: return 7; 
        case 1:  return 8; 
        case 2:  return 9;
        case 3:  return 10;
        default: return 0;
	}

}

int Carta::getValor() const{
	return this->valor;
}

Naipe Carta::getNaipe() const{
	return this->naipe;
}

int Carta::getForcaNaipe() const{
	switch(this->naipe) {
		case Naipe::paus:	 return 4;
		case Naipe::copas:   return 3;
		case Naipe::espadas: return 2;
		case Naipe::ouros:	 return 1;
		default:			 return 0;
	}
}