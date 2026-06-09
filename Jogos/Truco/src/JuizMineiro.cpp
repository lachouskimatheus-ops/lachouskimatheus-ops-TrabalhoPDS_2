#include "JuizMineiro.hpp"

// Força base das cartas no Truco (sem manilha)
static int forcaBase(int valor) {
    switch (valor) {
        case 4:  return 1;
        case 5:  return 2;
        case 6:  return 3;
        case 7:  return 4;
        case 12: return 5; // Q
        case 11: return 6; // J
        case 13: return 7; // K
        case 1:  return 8; // A
        case 2:  return 9;
        case 3:  return 10;
        default: return 0;
    }
}

static int forcaNaipe(Naipe n) {
    switch (n) {
        case Naipe::ouros:   return 1;
        case Naipe::espadas: return 2;
        case Naipe::copas:   return 3;
        case Naipe::paus:    return 4;
        default:             return 0;
    }
}

// Manilhas fixas do Truco Mineiro (da mais fraca para a mais forte):
// 7 de ouros (97), A de espadas (98), 7 de copas (99), 4 de paus (100)
static int forcaManilhaMineiro(int valor, Naipe naipe) {
    if (valor == 7 && naipe == Naipe::ouros)    return 97;
    if (valor == 1 && naipe == Naipe::espadas)  return 98;
    if (valor == 7 && naipe == Naipe::copas)    return 99;
    if (valor == 4 && naipe == Naipe::paus)     return 100;
    return -1; // não é manilha
}

int JuizMineiro::decidirVencedor(std::vector<Carta*> cartasNaMesa, Carta vira, bool forcarVencedor) {

    auto calcForca = [&](const Carta* c) -> int {
        int fm = forcaManilhaMineiro(c->getValor(), c->getNaipe());
        if (fm > 0) return fm;
        return forcaBase(c->getValor());
    };

    int indiceVencedor = -1;
    int maiorForca = -1;
    bool empate = false;

    for (int i = 0; i < (int)cartasNaMesa.size(); i++) {
        int forca = calcForca(cartasNaMesa[i]);

        if (forca > maiorForca) {
            maiorForca = forca;
            indiceVencedor = i;
            empate = false;
        } else if (forca == maiorForca) {
            if (forcarVencedor) {
                // Desempata pelo naipe
                if (forcaNaipe(cartasNaMesa[i]->getNaipe()) > forcaNaipe(cartasNaMesa[indiceVencedor]->getNaipe())) {
                    indiceVencedor = i;
                    empate = false;
                }
            } else {
                empate = true;
            }
        }
    }

    return empate ? -1 : indiceVencedor;
}