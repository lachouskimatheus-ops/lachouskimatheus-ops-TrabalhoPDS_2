jogador 21.cpp
#include "Jogador21.hpp"

Jogador21::Jogador21(int id, const std::string& nome)
    : Jogador(id, nome),
      pontuacaoAtual_(0),
      parou_(false) {
}

int Jogador21::calcularPontuacao() {
    int soma = 0;
    int quantidadeAses = 0;

    for (Carta* carta : mao_) {
        if (carta == nullptr) {
            continue;
        }

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

bool Jogador21::estourou() {
    return calcularPontuacao() > 21;
}

int Jogador21::getPontuacao() const {
    return pontuacaoAtual_;
}

void Jogador21::parar() {
    parou_ = true;
}

bool Jogador21::parou() const {
    return parou_;
}

void Jogador21::prepararNovaRodada() {
    for (Carta* carta : mao_) {
        delete carta;
    }

    mao_.clear();
    pontuacaoAtual_ = 0;
    parou_ = false;
}

const std::vector<Carta*>& Jogador21::verMao() const {
    return mao_;
}
