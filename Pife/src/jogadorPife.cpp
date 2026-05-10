#include "jogadorPife.h"

void JogadorPife::receberCarta(const Carta& carta) { //adiona a carta ao final do vetor 'mao'
    mao.push_back(carta);
}

int JogadorPife::tmnhMao() const { //retorna tamnaho do vetor  'mao'
    return mao.size();
}

void JogadorPife::organizarMao(){
    for(int i = 0; i < mao.size() - 1; i++){
        for(int j = 0; j < mao.size() - 1 -i; j++){
            int valorAtual = static_cast<int>(mao[j].mostraValor()); //transforma o tip valor em int

            int valorProximo = static_cast<int>(mao[j+1].mostraValor());

            if(valorAtual > valorProximo){
                Carta temp = mao[j];
                mao[j] = mao[j+1];
                mao[j+1] = temp;
            }
        }
    }
}

Carta JogadorPife::descartarCarta(int indice){
    if(indice < 0 || indice >= mao.size()){
        return Carta();
    }

    Carta descartada = mao[indice];
    mao.erase(mao.begin() + indice);
    return descartada;
}

const std::vector<Carta>& JogadorPife::verMao()const{
    return mao;
}