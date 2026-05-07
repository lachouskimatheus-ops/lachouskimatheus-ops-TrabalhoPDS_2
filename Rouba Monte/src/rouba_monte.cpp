#include "rouba_monte.h"
#include <iostream>

void Jogo::inicializarMesa(Baralho& b, Monte& mesa) {
    for(int i = 0; i < 8; i++) {
        if(!b.estaVazio()) mesa.push_back(b.retirarCarta());
    }
}

bool Jogo::tentaRoubar(Carta comprada, Monte& meuMonte, Monte& monteAdversario) {
    if (!monteAdversario.empty() && comprada.mostraValor() == monteAdversario.back().mostraValor()) {
        // Pega o monte do outro e coloca no seu
        meuMonte.insert(meuMonte.end(), monteAdversario.begin(), monteAdversario.end());
        meuMonte.push_back(comprada);
        monteAdversario.clear(); // O outro fica sem nada
        return true;
    }
    return false;
}

bool Jogo::tentaPegarMesa(Carta comprada, Monte& meuMonte, Monte& mesa) {
    for (auto it = mesa.begin(); it != mesa.end(); ++it) {
        if (comprada.mostraValor() == it->mostraValor()) {
            meuMonte.push_back(*it); // Pega a carta da mesa
            meuMonte.push_back(comprada); // Pega a que comprou
            mesa.erase(it); // Remove da mesa
            return true;
        }
    }
    return false;
}

void Jogo::descartar(Carta comprada, Monte& mesa) {
    mesa.push_back(comprada);
}

void Jogo::exibirEstado(const Monte& mesa, const Monte& m1, const Monte& m2) {
    std::cout << "\nMESA: ";
    for(const auto& c : mesa) std::cout << "[" << c.valorString() << "] ";
    std::cout << "\nMonte J1 (Topo): " << (m1.empty() ? "Vazio" : m1.back().cartaString());
    std::cout << "\nMonte J2 (Topo): " << (m2.empty() ? "Vazio" : m2.back().cartaString()) << "\n";
}
