#include "baralho.h"
#include <iostream>
#include <random>
#include <algorithm>


baralho::baralho() {

std::vector<Valor> valor = {
        Valor::As, Valor::Dois, Valor::Tres, Valor::Quatro,
        Valor::Cinco, Valor::Seis, Valor::Sete, Valor::Oito,
        Valor::Nove, Valor::Dez, Valor::Valete, Valor::Dama, Valor::Rei

};

std::vector<Naipe> naipe = {

    Naipe::Ouro, Naipe::Copa, Naipe::Espada, Naipe::Paus
};

for (int i = 0; i < naipe.size(); i++) {
    for (int j = 0; j < valor.size(); j++) {
        cartas.push_back(Carta(valor[j], naipe[i]));
    }
}

}