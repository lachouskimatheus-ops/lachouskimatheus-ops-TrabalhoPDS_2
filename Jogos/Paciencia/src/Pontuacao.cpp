#include "Pontuacao.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

Pontuacao::Pontuacao() : pontos(0), passadasCava(0), record(0) {
    carregarRecord();
}

void Pontuacao::resetar() {
    pontos = 0;
    passadasCava = 0;
}

void Pontuacao::aplicar(EventoPontuacao evento) {
    switch (evento) {
        case EventoPontuacao::CavaParaColuna:
            pontos += 5;
            break;
        case EventoPontuacao::CavaParaFundacao:
            pontos += 10;
            break;
        case EventoPontuacao::ColunaParaFundacao:
            pontos += 10;
            break;
        case EventoPontuacao::ColunaParaColuna:
            pontos += 5; // A lógica que você queria adicionar
            break;
        case EventoPontuacao::FundacaoParaColuna:
            pontos -= 15; // Penalidade por tirar da fundação
            break;
        case EventoPontuacao::VirarCarta:
            pontos += 5;
            break;
        case EventoPontuacao::PassarBaralho:
            // Opcional: penalidade por girar o deck completo
            if (passadasCava > 0) pontos -= 10;
            passadasCava++;
            break;
    }
    
    // Garante que pontos não fiquem negativos
    if (pontos < 0) pontos = 0;
}

// Persistência: Carregar do Arquivo
void Pontuacao::carregarRecord() {
    std::ifstream arq(ARQUIVO_RECORD);
    if (arq.is_open()) {
        arq >> record;
        arq.close();
    } else {
        record = 0; // Se não existir arquivo, record começa em 0
    }
}

// Persistência: Salvar no Arquivo
bool Pontuacao::salvarRecord() {
    if (pontos > record) {
        record = pontos;
        std::ofstream arq(ARQUIVO_RECORD);
        if (arq.is_open()) {
            arq << record;
            arq.close();
            return true;
        }
    }
    return false;
}

// Setters
void Pontuacao::setPassadasCava(int p) { passadasCava = p; }
void Pontuacao::setPontos(int p) { pontos = p; }