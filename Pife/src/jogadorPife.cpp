#include "jogadorPife.h"

JogadorPife::JogadorPife(){
    podeComprarMesa_ = true;
}

void JogadorPife::receberCarta(const Carta& carta) { //adiona a carta ao final do vetor 'mao'
    mao_.push_back(carta);
}

int JogadorPife::tmnhMao() const { //retorna tamnaho do vetor  'mao'
    return mao_.size();
}

void JogadorPife::organizarMao(){
    for(int i = 0; i < mao_.size() - 1; i++){
        for(int j = 0; j < mao_.size() - 1 -i; j++){
            int valorAtual = static_cast<int>(mao_[j].mostraValor()); //transforma o tip valor em int

            int valorProximo = static_cast<int>(mao_[j+1].mostraValor());

            if(valorAtual > valorProximo){
                Carta temp = mao_[j];
                mao_[j] = mao_[j+1];
                mao_[j+1] = temp;
            }
        }
    }
}

void JogadorPife::bloquearCompraMesa(){
    podeComprarMesa_ = false;
}

void JogadorPife::liberarCompraMesa(){
    podeComprarMesa_ = true;
}

bool JogadorPife::podeComprarMesa() const{
    return podeComprarMesa_;
}

Carta JogadorPife::descartarCarta(int indice){
    if(indice < 0 || indice >= mao_.size()){
        return Carta();
    }

    Carta descartada = mao_[indice];
    mao_.erase(mao_.begin() + indice);
    return descartada;
}

const std::vector<Carta>& JogadorPife::verMao()const{
    return mao_;
}

