#include <iostream>
#include <cassert>
#include <vector>
#include "paciencia.h"
#include "regras.h"
#include "baralho.h"
#include "cartas.h"

using namespace std;

// Funções Auxiliares de Teste
void testarBaralho() {
    cout << "Testando Baralho... ";
    Baralho b;
    assert(b.tamanho() == 52); // Um baralho deve ter 52 cartas
    
    Carta c = b.retirarCarta();
    assert(b.tamanho() == 51); // Deve diminuir após retirar
    cout << "OK!" << endl;
}

void testarRegrasMovimentacao() {
    cout << "Testando Regras de Movimentacao... ";
    
    // Criando cartas manualmente para testar (Valor, Naipe)
    Carta setePreto(Valor::Sete, Naipe::Espada);
    Carta oitoVermelho(Valor::Oito, Naipe::Copa);
    Carta oitoPreto(Valor::Oito, Naipe::Paus);
    Carta asOuro(Valor::As, Naipe::Ouro);
    Carta doisOuro(Valor::Dois, Naipe::Ouro);

    // 1. Testar coluna (7 preto sobre 8 vermelho = VALIDO)
    assert(Regras::podeMoverParaColuna(setePreto, oitoVermelho) == true);
    
    // 2. Testar coluna (8 preto sobre 8 vermelho = INVALIDO - cores iguais/valor igual)
    assert(Regras::podeMoverParaColuna(oitoPreto, oitoVermelho) == false);
    
    // 3. Testar fundação (2 de Ouro sobre As de Ouro = VALIDO)
    vector<Carta> pilhaFundacao;
    pilhaFundacao.push_back(asOuro);
    assert(Regras::podeMoverParaFundacao(doisOuro, pilhaFundacao) == true);
    
    // 4. Testar Rei na coluna vazia
    Carta rei(Valor::Rei, Naipe::Paus);
    assert(Regras::podeMoverParaColunaVazia(rei) == true);
    assert(Regras::podeMoverParaColunaVazia(asOuro) == false);

    cout << "OK!" << endl;
}

void testarPontuacao() {
    cout << "Testando Sistema de Pontuacao... ";
    Pontuacao p;
    p.resetar();
    
    p.aplicar(EventoPontuacao::ColunaParaFundacao); // +10
    assert(p.getPontos() == 10);
    
    p.aplicar(EventoPontuacao::FundacaoParaColuna); // -15
    assert(p.getPontos() == 0); // Não deve ficar negativo (conforme seu switch)
    
    cout << "OK!" << endl;
}

void testarEstadoDoJogo() {
    cout << "Testando Fluxo do Jogo... ";
    Paciencia jogo;
    
    int pontosIniciais = jogo.getPontuacao();
    assert(pontosIniciais == 0);
    
    // Testa se o desfazer funciona sem jogadas
    assert(jogo.desfazer() == false);
    
    cout << "OK!" << endl;
}

int main() {
    cout << "=== INICIANDO TESTES DO PACIENCIA ===" << endl;
    
    try {
        testarBaralho();
        testarRegrasMovimentacao();
        testarPontuacao();
        testarEstadoDoJogo();
        
        cout << "\n=====================================" << endl;
        cout << "  TODOS OS TESTES PASSARAM COM SUCESSO!" << endl;
        cout << "=====================================" << endl;
    } catch (...) {
        cerr << "\nERRO FATAL: Um dos testes falhou!" << endl;
        return 1;
    }

    return 0;
}