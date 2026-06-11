#include "BaralhoSujo.hpp"
#include "Carta.hpp"

BaralhoSujo::BaralhoSujo() {
    inicializar();
}

void BaralhoSujo::inicializar(){
    //Limpa as cartas antigas da memória antes de recriar o baralho
    for(size_t i = 0; i < cartas_.size(); i++) {
        delete cartas_[i];
    }
    cartas_.clear();

    // Cria as 40 cartas do truco/FDP
    Naipe naipes[] = {Naipe::paus, Naipe::copas, Naipe::espadas, Naipe::ouros};

    for (Naipe naipe : naipes) {
        for(int i = 1; i <= 13; i++) {
            if(i == 8 || i == 9 || i == 10) continue;
            Carta* novaCarta = new Carta(i, naipe);
            cartas_.push_back(novaCarta);
        }
    }
}