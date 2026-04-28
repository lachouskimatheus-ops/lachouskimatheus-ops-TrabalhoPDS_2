#include "baralho.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <ctime>

Baralho::Baralho() {

std::vector<Valor> valor = {
        Valor::As, Valor::Dois, Valor::Tres, Valor::Quatro,
        Valor::Cinco, Valor::Seis, Valor::Sete, Valor::Oito,
        Valor::Nove, Valor::Dez, Valor::Valete, Valor::Dama, Valor::Rei

};

std::vector<Naipe> naipe = {

    Naipe::Ouro, Naipe::Copa, Naipe::Espada, Naipe::Paus
};
//percorre as cartas tudo e atribui
for (int i = 0; i < naipe.size(); i++) {
    for (int j = 0; j < valor.size(); j++) {
        cartas.push_back(Carta(valor[j], naipe[i]));
    }
}}


void Baralho::embaralhar() {
    std::srand(std::time(0)); 

    for (int i = 0; i < cartas.size(); i++) {
        int j = std::rand() % cartas.size();

   
        Carta temp = cartas[i];
        cartas[i] = cartas[j];
        cartas[j] = temp;
    }
}
Carta Baralho::retirarCarta(){

if (estaVazio())
{
    std::cout<< "Baralho Vazio!!!"<< std::endl;
    return Carta();
}


Carta primeira = cartas.front();
cartas.erase(cartas.begin());

return primeira;}

int Baralho::tamanho() const{
    return cartas.size();
}
bool Baralho::estaVazio() const{
    return cartas.empty();
}








