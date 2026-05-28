#include "Carta.hpp"

// Construtor Padrão (Truco)
Carta::Carta() : valor_(0), naipe_(Naipe::Nenhum) {}

// Construtor Numérico (FDP)
Carta::Carta(int valor, Naipe naipe) {
    this->valor_ = valor;
    this->naipe_ = naipe;
}

// Construtor Enum (Truco)
Carta::Carta(Valor valor, Naipe naipe) {
    this->valor_ = static_cast<int>(valor);
    this->naipe_ = naipe;
}

// ==========================================
// IMPLEMENTAÇÕES DE RETORNO E VALIDAÇÃO
// ==========================================

int Carta::getValor() const {
    return valor_;
}

Naipe Carta::getNaipe() const {
    return naipe_;
}

Valor Carta::mostraValor() const {
    return static_cast<Valor>(valor_);
}

Naipe Carta::mostraNaipe() const {
    return naipe_;
}

bool Carta::validacaoCarta() const {
    return valor_ != 0 && naipe_ != Naipe::Nenhum;
}

// ==========================================
// IMPLEMENTAÇÕES DE LÓGICA E FORÇA
// ==========================================

int Carta::forca() const {
    return valor_; 
}

int Carta::getForcaNaipe() const {
    if (naipe_ == Naipe::ouros) { return 1; };
    if (naipe_ == Naipe::espadas) { return 2; };
    if (naipe_ == Naipe::copas) { return 3; };
    if (naipe_ == Naipe::paus) { return 4; };
    return 0;
}

// ==========================================
// IMPLEMENTAÇÕES DE TEXTO E JSON
// ==========================================

std::string Carta::toString() const {
    std::string valor;
    switch(valor_) {
        case 1:  valor = "A"; break;
        case 11: valor = "J"; break;
        case 12: valor = "Q"; break;
        case 13: valor = "K"; break;
        default: valor = std::to_string(valor_); break;
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

std::string Carta::valorString() const {
    switch (static_cast<Valor>(valor_)) {
        case Valor::As: return "As";
        case Valor::Dois: return "2";
        case Valor::Tres: return "3";
        case Valor::Quatro: return "4";
        case Valor::Cinco: return "5";
        case Valor::Seis: return "6";
        case Valor::Sete: return "7";
        case Valor::Oito: return "8";
        case Valor::Nove: return "9";
        case Valor::Dez: return "10";
        case Valor::Valete: return "Valete";
        case Valor::Dama: return "Dama";
        case Valor::Rei: return "Rei";
        default: return "Invalido";
    };
}

std::string Carta::naipeString() const {
    switch (naipe_) {
        case Naipe::Ouro: return "Ouro";
        case Naipe::Copa: return "Copa";
        case Naipe::Espada: return "Espada";
        case Naipe::Paus: return "Paus";
        default: return "Nenhum";
    };
}

std::string Carta::cartaString() const {
    if (validacaoCarta() == false) {
        return "Carta Inválida";
    };
    return valorString() + " de " + naipeString();
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
}