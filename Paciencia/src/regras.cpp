#include "regras.h"


// Método privado: verifica a cor 
bool Regras::ehVermelha(const Carta& c) {
    // Ajustado para usar os nomes do seu baralho.cpp: Naipe::Copa e Naipe::Ouro
    return (c.getNaipe() == Naipe::Copa || c.getNaipe() == Naipe::Ouro);
}

// Valida o movimento entre colunas da mesa.

bool Regras::podeMoverParaColuna(const Carta& origem, const Carta& destino) {
    if (ehVermelha(origem) == ehVermelha(destino)) {
        return false;
    }

    return ((int)origem.getValor() == (int)destino.getValor() - 1);
}

// Valida o movimento para as pilhas de fundação.

bool Regras::podeMoverParaFundacao(const Carta& carta, const std::vector<Carta>& pilha) {
    if (pilha.empty()) {
        // Apenas o Ás pode iniciar a fundação
        return (carta.getValor() == Valor::As);
    }

    const Carta& topo = pilha.back();
    
    // Deve ser do mesmo naipe e o valor seguinte
    return (carta.getNaipe() == topo.getNaipe() && 
            (int)carta.getValor() == (int)topo.getValor() + 1);
}

// Valida se um Rei pode entrar em uma coluna vazia.

bool Regras::podeMoverParaColunaVazia(const Carta& carta) {
    return (carta.getValor() == Valor::Rei);
}