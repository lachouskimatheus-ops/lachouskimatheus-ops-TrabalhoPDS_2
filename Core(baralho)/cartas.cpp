#include "cartas.h"

Carta::Carta() : valor_(Valor::Invalido), naipe_(Naipe::Nenhum){    
}//evita buff de memória

Carta::Carta(Valor valor, Naipe naipe) : valor_(valor), naipe_(naipe) {
}//função construtora de cada carta, atribui o naipe e o valor

Valor Carta::mostraValor() const {
    return valor_;
}

Naipe Carta::mostraNaipe() const {
    return naipe_;
}

bool Carta::validacaoCarta() const{ //filtra uma carta Invalida
    return valor_!= Valor::Invalido && naipe_!= Naipe::Nenhum;
}

std::string Carta::valorString() const { //flutua até achar um que seja verdadeiro
    switch (valor_){
        case Valor::As:
            return "As";

        case Valor::Dois:
            return "2";
        
        case Valor::Tres:
            return "3";

        case Valor::Quatro:
            return "4";

        case Valor::Cinco:
            return "5";

        case Valor::Seis:
            return "6";

        case Valor::Sete:
            return "7";

        case Valor::Oito:
            return "8";

        case Valor::Nove:
            return "9";

        case Valor::Dez:
            return "10";

        case Valor::Valete:
            return "Valete";

        case Valor::Dama:
            return "Dama";

        case Valor::Rei:
            return "Rei";
        
        default:
            return "Invalido";
    }
}

std::string Carta::naipeString() const{ //flutua até achar um que seja verdadeiro
    switch (naipe_){
        case Naipe::Ouro:
            return "Ouro";
        
        case Naipe::Copa:
            return "Copa";

        case Naipe::Espada:
            return "Espada";

        case Naipe::Paus:
            return "Paus";

        default:
            return "Nenhum";
    }
}

std::string Carta::cartaString() const {
    if(validacaoCarta() == false){
        return "Carta Inválida";
    }
    return valorString() + " " + "de" + " " + naipeString();
}