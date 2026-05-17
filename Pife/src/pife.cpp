#include "pife.h"

Pife::Pife(int quantidadeJogadores){ //construtor do Pife, já distribui as cartas a partir no número de jogadores 
    jogadorAtual_ = 0;
    jogoFinalizado_ = false;
    baralho_ = Baralho(2);
    for(int i = 0; i < quantidadeJogadores; i++){
        jogadores_.push_back(JogadorPife());
    }

    baralho_.embaralhar();
    distribuirCartas();
}

void Pife::comprarBaralho(){ //é a ação de pegar a carta que o baralho.h dá, e entrega para a mao do JogadorPife.h
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

void Pife::comprarMesa(){
    if(!jogadores_[jogadorAtual_].podeComprarMesa()) return;

    if(mesa_.empty()) return;

    Carta carta = mesa_.back(); //copia a ultima carta da mesa
    mesa_.pop_back();//apaga essa carta do vetor para não ter duplicata

    jogadores_[jogadorAtual_].receberCarta(carta); //usa a classe JogadorPife para entregar a carta
}

void Pife::distribuirCartas(){
    for(int carta = 0; carta<9; carta++){
        for(int i =0; i < static_cast<int>(jogadores_.size()); i++){
            jogadorAtual_ = i;
            comprarBaralho();
        }
    }
    jogadorAtual_ = 0;
}

void Pife::proximoJogador(){
    jogadorAtual_++;
    if(jogadorAtual_ >= static_cast<int>(jogadores_.size())){
        jogadorAtual_ = 0;
    }
}

bool Pife::bati(){
    if(verificaVitoria(jogadores_[jogadorAtual_].verMao())){
        jogoFinalizado_ = true;
        return true;
    }
    else{
        jogadores_[jogadorAtual_].bloquearCompraMesa();

        return false;
    }

}

bool Pife::jogoFinalizado() const{
    return jogoFinalizado_;
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