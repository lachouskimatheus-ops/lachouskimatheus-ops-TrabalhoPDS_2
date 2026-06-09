#include "JuizPaulistaTruco.hpp"

int JuizPaulistaTruco::forcaNaipe(Naipe naipe) const {
    switch (naipe) {
        case Naipe::ouros: return 1;
        case Naipe::espadas: return 2;
        case Naipe::copas: return 3;
        case Naipe::paus: return 4;
        default: return 0;
    }
}

int JuizPaulistaTruco::calcularValorManilha(int valorVira) const {
    switch (valorVira) {
        case 4: return 5;
        case 5: return 6;
        case 6: return 7;
        case 7: return 12;
        case 12: return 11;
        case 11: return 13;
        case 13: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        default: return -1;
    }
}

int JuizPaulistaTruco::valorCarta(const Carta& carta, const Carta& vira) const {
    int valorManilha = calcularValorManilha(vira.getValor());

    if (carta.getValor() == valorManilha) {
        return 100 + forcaNaipe(carta.getNaipe());
    }

    switch (carta.getValor()) {
        case 4: return 1;
        case 5: return 2;
        case 6: return 3;
        case 7: return 4;
        case 12: return 5;
        case 11: return 6;
        case 13: return 7;
        case 1: return 8;
        case 2: return 9;
        case 3: return 10;
        default: return 0;
    }
}

int JuizPaulistaTruco::decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor) const {
    int indiceVencedor = -1;
    int maiorForca = -1;
    bool empate = false;

    for (int i = 0; i < static_cast<int>(cartas.size()); ++i) {
        if (cartas[i] == nullptr) continue;

        int forca = valorCarta(*cartas[i], vira);

        if (forca > maiorForca) {
            maiorForca = forca;
            indiceVencedor = i;
            empate = false;
        } else if (forca == maiorForca) {
            if (forcarVencedor && indiceVencedor >= 0) {
                int naipeAtual = forcaNaipe(cartas[i]->getNaipe());
                int naipeVencedor = forcaNaipe(cartas[indiceVencedor]->getNaipe());

                if (naipeAtual > naipeVencedor) {
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