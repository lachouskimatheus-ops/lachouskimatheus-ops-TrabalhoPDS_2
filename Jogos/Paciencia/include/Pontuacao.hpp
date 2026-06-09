#ifndef PONTUACAO_H
#define PONTUACAO_H

#include <string>

// Enum class garante Type Safety (não permite misturar com inteiros)
enum class EventoPontuacao {
    CavaParaColuna,
    CavaParaFundacao,
    ColunaParaFundacao,
    FundacaoParaColuna,
    VirarCarta,
    ColunaParaColuna,
    PassarBaralho
};

class Pontuacao {
private:
    int pontos;
    int passadasCava;
    int record;

    // static constexpr é melhor que const std::string membro
    static constexpr const char* ARQUIVO_RECORD = "record.txt";

    // Método auxiliar privado (encapsulado)
    void carregarRecord();

public:
    Pontuacao();

    //  Lógica de Negócio 
    // Aplica a pontuação baseada em um evento do jogo
    void aplicar(EventoPontuacao evento);
    
    // Reseta o estado da pontuação atual
    void resetar();
    
    // Persistência
    bool salvarRecord();

    //  Getters (Inline para performance) 
    int getPontos() const { return pontos; }
    int getRecord() const { return record; }
    int getPassadasCava() const { return passadasCava; }

    //  Setters (Usados principalmente para restaurar estado de salvamento) 
    void setPassadasCava(int p);
    void setPontos(int p);
};

#endif