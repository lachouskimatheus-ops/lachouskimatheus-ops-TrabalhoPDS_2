#include "Carta.hpp"

Carta::Carta(int valor, Naipe naipe){
	this->valor_ = valor;
	this->naipe_ = naipe;
}

std::string Carta::toString() const {

    std::string valor;

    switch(valor_) {
        case 1:  valor = "A"; break;
        case 11: valor = "J"; break;
        case 12: valor = "Q"; break;
        case 13: valor = "K"; break;
        default: valor = std::to_string(valor_); break; //Transforma o int em string
    };

    std::string naipe;

    switch(naipe_) {
        case Naipe::ouros:   naipe = "Ouros ♦"; break;
        case Naipe::espadas: naipe = "Espadas ♠"; break;
        case Naipe::copas:   naipe = "Copas ♥"; break;
        case Naipe::paus:    naipe = "Paus ♣"; break;
        default: naipe = "?"; break;
    };

	return valor + " de " + naipe;
}

int Carta::getValor() const {
	return valor_;
}

Naipe Carta::getNaipe() const {
	return naipe_;
}

int Carta::forca() const {
    return valor_; 
}

int Carta::getForcaNaipe() const {
    if (naipe_ == Naipe::ouros) return 1;
    if (naipe_ == Naipe::espadas) return 2;
    if (naipe_ == Naipe::copas) return 3;
    if (naipe_ == Naipe::paus) return 4;
    return 0;
}

nlohmann::json Carta::paraJson() const {
    nlohmann::json dadosCarta;
    
    dadosCarta["valor"] = valor_;
    
    if (naipe_ == Naipe::paus) {
        dadosCarta["naipe"] = "paus";
    };
    if (naipe_ == Naipe::copas) {
        dadosCarta["naipe"] = "copas";
    };
    if (naipe_ == Naipe::espadas) {
        dadosCarta["naipe"] = "espadas";
    };
    if (naipe_ == Naipe::ouros) {
        dadosCarta["naipe"] = "ouros";
    };
    
    return dadosCarta;
};