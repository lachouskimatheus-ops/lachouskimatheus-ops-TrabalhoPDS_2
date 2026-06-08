#include "Regras.h"

// Verifica se a carta pode ir para a fundação
bool Regras::podeMoverParaFundacao(const Carta& carta, const std::vector<Carta>& fundacao) {
    if (fundacao.empty()) {
        // Fundação vazia só aceita Ás (valor 1)
        return carta.getValor() == 1;
    }

    const Carta& topo = fundacao.back();
    
    // Deve ser do mesmo naipe e exatamente um valor acima
    return (carta.getNaipe() == topo.getNaipe()) && 
           (carta.getValor() == topo.getValor() + 1);
}

// Verifica se a carta pode ir para uma coluna (tableau)
bool Regras::podeMoverParaColuna(const Carta& carta, const std::vector<Carta>& coluna) {
    if (coluna.empty()) {
        // Coluna vazia só aceita Rei (valor 13)
        return carta.getValor() == 13;
    }

    const Carta& topo = coluna.back();

    // Deve ser cor diferente (ex: Vermelho vs Preto) e exatamente um valor abaixo
    return (coresDiferentes(carta, topo)) && 
           (carta.getValor() == topo.getValor() - 1);
}

// Função auxiliar privada para checar cores
bool Regras::coresDiferentes(const Carta& c1, const Carta& c2) {
    bool c1Vermelha = (c1.getNaipe() == Naipe::Copa || c1.getNaipe() == Naipe::Ouro);
    bool c2Vermelha = (c2.getNaipe() == Naipe::Copa || c2.getNaipe() == Naipe::Ouro);
    
    // Retorna true apenas se forem cores diferentes
    return c1Vermelha != c2Vermelha;
}