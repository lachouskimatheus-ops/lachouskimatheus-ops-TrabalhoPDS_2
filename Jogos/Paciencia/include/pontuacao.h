#ifndef PONTUACAO_H
#define PONTUACAO_H
#include <string>

enum class EventoPontuacao {
    CavaParaColuna,
    CavaParaFundacao,
    ColunaParaFundacao,
    FundacaoParaColuna,
    VirarCarta,
    PassarBaralho
};

class Pontuacao {
private:
    int pontos;
    int passadasCava;
    int record;
    const std::string ARQUIVO_RECORD = "record.txt";
    void carregarRecord();

public:
    Pontuacao();
    void aplicar(EventoPontuacao evento);
    int getPontos() const;
    int getRecord() const;
    bool salvarRecord();
    void resetar();
    int getPassadasCava() const;
    void setPassadasCava(int p);
    void setPontos(int p);
    
};

#endif