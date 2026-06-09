#include "JuizPaulista.hpp"

// Força base das cartas no Truco (sem manilha)
// Ordem da mais fraca para mais forte: 4, 5, 6, 7, Q(12), J(11), K(13), A(1), 2, 3
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

// Força do naipe (do mais fraco para o mais forte): ouros, espadas, copas, paus
static int forcaNaipe(Naipe n) {
    switch (n) {
        case Naipe::ouros:   return 1;
        case Naipe::espadas: return 2;
        case Naipe::copas:   return 3;
        case Naipe::paus:    return 4;
        default:             return 0;
    }
}

int JuizPaulista::decidirVencedor(std::vector<Carta*> cartasNaMesa, Carta vira, bool forcarVencedor) {

    // Define a manilha: carta diretamente superior à vira na ordem base
    int cVira = vira.getValor();
    int valorManilha;
    switch (cVira) {
        case 4:  valorManilha = 5;  break;
        case 5:  valorManilha = 6;  break;
        case 6:  valorManilha = 7;  break;
        case 7:  valorManilha = 12; break; // 7 -> Q
        case 12: valorManilha = 11; break; // Q -> J
        case 11: valorManilha = 13; break; // J -> K
        case 13: valorManilha = 1;  break; // K -> A
        case 1:  valorManilha = 2;  break; // A -> 2
        case 2:  valorManilha = 3;  break; // 2 -> 3
        case 3:  valorManilha = 4;  break; // 3 -> 4
        default: valorManilha = -1; break;
    }

    // Calcula a força de cada carta
    // Manilhas valem 100+ (ordenadas por naipe)
    // Não-manilhas valem pela tabela base
    auto calcForca = [&](const Carta* c) -> int {
        if (c->getValor() == valorManilha) {
            return 100 + forcaNaipe(c->getNaipe()); // 101-104
        }
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