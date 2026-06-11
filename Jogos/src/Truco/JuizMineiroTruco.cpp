#include "JuizMineiroTruco.hpp"

int JuizMineiroTruco::forcaNaipe(Naipe naipe) const {
    switch (naipe) {
        case Naipe::ouros: return 1;
        case Naipe::espadas: return 2;
        case Naipe::copas: return 3;
        case Naipe::paus: return 4;
        default: return 0;
    }
}

int JuizMineiroTruco::valorCarta(const Carta& carta) const {
    int valor = carta.getValor();
    Naipe naipe = carta.getNaipe();

    if (valor == 7 && naipe == Naipe::ouros) return 97;
    if (valor == 1 && naipe == Naipe::espadas) return 98;
    if (valor == 7 && naipe == Naipe::copas) return 99;
    if (valor == 4 && naipe == Naipe::paus) return 100;

    switch (valor) {
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

int JuizMineiroTruco::decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor) const {
    (void)vira;

    int indiceVencedor = -1;
    int maiorForca = -1;
    bool empate = false;

    for (int i = 0; i < static_cast<int>(cartas.size()); ++i) {
        if (cartas[i] == nullptr) continue;

        int forca = valorCarta(*cartas[i]);

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