#include "Jogador21.hpp"
#include "Carta.hpp"

Jogador21::Jogador21(int id, std::string nome) : Jogador(id, nome), pontuacaoAtual_(0) {}

int Jogador21::calcularPontuacao() {
    int soma = 0;
    int quantidadeAses = 0;

    for (Carta* carta : mao_) {
        int valorCarta = carta->getValor();

        if (valorCarta == 1) { 
            quantidadeAses++;
            soma += 11;
        } else if (valorCarta >= 11 && valorCarta <= 13) { 
            soma += 10;
        } else {
            soma += valorCarta;
        }
    }

    while (soma > 21 && quantidadeAses > 0) {
        soma -= 10; 
        quantidadeAses--;
    }

    pontuacaoAtual_ = soma;
    return pontuacaoAtual_;
}

bool Jogador21::estourou() const {
    return pontuacaoAtual_ > 21;
}

int Jogador21::getPontuacao() const {
    return pontuacaoAtual_;
}
