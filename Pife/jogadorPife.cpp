#include "jogadorPife.h"

void JogadorPife::receberCarta(const Carta& carta) { //adiona a carta ao final do vetor 'mao'
    mao.push_back(carta);
}

int JogadorPife::tmnhMao() const { //retorna tamnaho do vetor  'mao'
    return sizeof.mao();
}

