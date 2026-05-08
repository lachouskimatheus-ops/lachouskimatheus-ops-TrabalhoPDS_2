#include "Jogador_Truco.hpp"
#include <iostream>

Jogador_Truco::Jogador_Truco(std::string nome) {
    this->nome = nome;
}

void Jogador_Truco::receberCarta(Carta* novacarta) {
    this->mao.push_back(novacarta);
}

void Jogador_Truco::mostrarmao() {
    for (int i = 0; i < (int)mao.size(); i++) {
        std::cout << "[" << i << "] ";
        mao[i]->imprimir(); 
    }
}

Carta* Jogador_Truco::jogarCarta(int indice) {
    if (indice < 0 || indice >= (int)mao.size()) {
        return nullptr;
    }

    Carta* cartaParaJogar = mao[indice];
    
    mao.erase(mao.begin() + indice);
    
    return cartaParaJogar;
}

void Jogador_Truco::limparMao() {
    this->mao.clear();
}

std::string Jogador_Truco::getNome() const {
    return this->nome;
}