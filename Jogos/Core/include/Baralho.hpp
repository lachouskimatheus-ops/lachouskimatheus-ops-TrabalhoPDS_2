#pragma once
#ifndef BARALHO_HPP
#define BARALHO_HPP

#include <iostream>
#include <vector>
#include <random>
#include "Carta.hpp" // Usa a nova Carta unificada que criamos antes

class Baralho {
protected:
    // O vetor central agora usa sempre ponteiros para facilitar a herança do FDP
    std::vector<Carta*> cartas_;

public:
    // Construtor: se for > 0, cria o baralho padrão de 52 cartas do Pife
    Baralho(int quantidadeBaralhos = 1);
    
    // Destrutor virtual para limpar a memória dos ponteiros
    virtual ~Baralho();

    // ==========================================
    // Métodos Originais do FDP (Classe Mãe)
    // ==========================================
    virtual void inicializar(); // Deixou de ser "= 0" para o Pife poder instanciar
    Baralho(const Baralho& outro);
    Baralho& operator=(const Baralho& outro);
    void embaralhar();
    Carta* puxarCarta();
    int getQtdCartas() const;

    // ==========================================
    // Métodos Originais do Pife / Antigos
    // ==========================================
    void inserirCarta(const Carta& c);
    Carta retirarCarta();
    int tamanho() const;
    bool estaVazio() const;
    void limpar();
};

#endif