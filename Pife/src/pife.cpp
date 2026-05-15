#include "pife.h"

Pife::Pife(int quantidadeJogadores){ //construtor do Pife, já distribui as cartas a partir no número de jogadores 
    jogadorAtual_ = 0;
    baralho_ = Baralho(2);
    for(int i = 0; i < quantidadeJogadores; i++){
        jogadores_.push_back(JogadorPife());
    }

    baralho_.embaralhar();
    distribuirCartas();
}

void Pife::comprarCarta(){ //é a ação de pegar a carta que o baralho.h dá, e entrega para a mao do JogadorPife.h
    if(baralho_.estaVazio()){
        return;
    }
    jogadores_[jogadorAtual_].receberCarta(baralho_.retirarCarta());
}

void Pife::colocarNaMesa(int indice){ //pega o descarte e coloca na mesa
    Carta descarte = jogadores_[jogadorAtual_].descartarCarta(indice);

    if(descarte.validacaoCarta()){
        mesa_.push_back(descarte);
    }

}

void Pife::distribuirCartas(){
    for(int carta = 0; carta<9; carta++){
        for(int i =0; i < jogadores_.size(); i++){
            jogadorAtual_ = i;
            comprarCarta();
        }
    }
    jogadorAtual_ = 0;
}

void Pife::proximoJogador(){
    jogadorAtual_++;
    if(jogadorAtual_ >= jogadores_.size()){
        jogadorAtual_ = 0;
    }
}

JogadorPife& Pife::consultarJogador(){
    return jogadores_[jogadorAtual_];

}

const std::vector<Carta>& Pife::consultarMesa() const {
    return mesa_;
}

int Pife::consultarIndiceJogadorAtual() const {
    return jogadorAtual_;
}

int Pife::numeroDejogadores() const{
    return jogadores_.size();
}