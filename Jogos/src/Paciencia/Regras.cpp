#include "Regras.hpp"

// Verifica movimento para o tabuleiro (Colunas)
bool Regras::podeMoverParaColuna(const Carta& origem, const Carta& destino) {
    // Regra: Uma unidade menor E cor oposta
    bool valorCorreto = (origem.getValor() == destino.getValor() - 1);
    bool coresDiferentes = (ehVermelha(origem) != ehVermelha(destino));
    
    return valorCorreto && coresDiferentes;
}

// Verifica movimento para a fundação (Pilha do topo)
bool Regras::podeMoverParaFundacao(const Carta& carta, const std::vector<Carta>& pilha) {
    if (pilha.empty()) {
        // Regra: Fundação vazia só aceita Ás (1)
        return carta.getValor() == 1;
    }

    // Regra: Deve ser o mesmo naipe e uma unidade maior
    const Carta& topo = pilha.back();
    return (carta.getNaipe() == topo.getNaipe()) && 
           (carta.getValor() == topo.getValor() + 1);
}

// Verifica movimento para uma coluna vazia
bool Regras::podeMoverParaColunaVazia(const Carta& carta) {
    // Regra: Apenas Reis (13) podem ocupar espaços vazios
    return carta.getValor() == 13;
}

// Auxiliar para verificação de cores
bool Regras::ehVermelha(const Carta& c) {
    return (c.getNaipe() == Naipe::Copa || c.getNaipe() == Naipe::Ouro);
}