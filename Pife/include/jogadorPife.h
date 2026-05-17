#ifndef JOGADORPIFE_H
#define JOGADORPIFE_H

#include <cartas.h>
#include <vector>
#include <string.h>

//A ideia é o jogadorPife.h funcionas apenas como um vetor da mão do jogador
//e algumas de suas ações como receberCarta e descartarCarta.

class JogadorPife {
private:
    std::vector<Carta> mao_; //vetor do tipo carta que se chama 'mao_'
    bool podeComprarMesa_;

public:
    JogadorPife();

    void receberCarta(const Carta& carta); //apenas recebe a carta que vai ser comprada pelo pife.h
    void organizarMao(); //Organiza a mão do jogador 
    void bloquearCompraMesa();
    void liberarCompraMesa();

    bool podeComprarMesa() const;

    int tmnhMao() const;
    Carta descartarCarta(int indice); //passa o indice da carta a ser descartada, e ent retana essa carta

    const std::vector<Carta>& verMao() const; //retorna o vetor 'mao', mas sem poder alterar
};
#endif