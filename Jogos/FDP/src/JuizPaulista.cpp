#include "JuizPaulista.hpp"

int JuizPaulista::obterPesoNaipeFDP(Naipe n) {
    switch (n) {
        case Naipe::paus: return 4;    // Zap (Mais forte)
        case Naipe::copas: return 3;   // Copeta
        case Naipe::espadas: return 2; // Espadilha
        case Naipe::ouros: return 1;   // Pica-fumo (Mais fraco)
        default: return 0;
    }
}

int JuizPaulista::obterForcaNormalFDP(int valor) {
    // Retorna uma pontuação arbitrária para garantir a hierarquia correta
    switch (valor) {
        case 3: return 10;
        case 2: return 9;
        case 1: return 8;  // As
        case 13: return 7; // Rei (K)
        case 11: return 6; // Valete (J)
        case 12: return 5; // Dama (Q)
        case 7: return 4;
        case 6: return 3;
        case 5: return 2;
        case 4: return 1;
        default: return 0;
    }
}

int JuizPaulista::decidirVencedor(std::vector<Carta*> cartasNaMesa, Carta vira) {

    int indiceVencedor = -1;
    int maiorForcaTotal = -1;

    for (size_t i = 0; i < cartasNaMesa.size(); i++){
        int forcaDaCarta;
        int v = cartasNaMesa[i]->getValor();
        Naipe n = cartasNaMesa[i]->getNaipe();
        int cVira = vira.getValor();
        int manilha;

            //Definicao Manilha
        switch (cVira){
            case 1: manilha = 2; break;
            case 2: manilha = 3; break;
            case 3: manilha = 4; break;
            case 4: manilha = 5; break;
            case 5: manilha = 6; break;
            case 6: manilha = 7; break;
            case 7: manilha = 12; break;
            case 12: manilha = 11; break;
            case 11: manilha = 13; break;
            case 13: manilha = 1; break;
            default: manilha = -1; break;
        }
            //Forca Manilhas
        if (v == manilha && n == Naipe::paus)          {forcaDaCarta = 100;}
        else if (v == manilha && n == Naipe::copas)    {forcaDaCarta = 99;}
        else if (v == manilha && n == Naipe::espadas)  {forcaDaCarta = 98;}
        else if (v == manilha && n == Naipe::ouros)    {forcaDaCarta = 97;}
        else {
            forcaDaCarta = obterForcaNormalFDP(v); // Forca Normal convertida para FDP
        }

        //Comparacao Forcas
        if (forcaDaCarta > maiorForcaTotal) {
            maiorForcaTotal = forcaDaCarta;
            indiceVencedor = i;
        } 
        else if (forcaDaCarta == maiorForcaTotal) {
            // Em caso de forca igual, OBRIGATORIAMENTE desempata pelo naipe do FDP
            int pesoNaipeAtual = obterPesoNaipeFDP(n);
            int pesoNaipeVencedor = obterPesoNaipeFDP(cartasNaMesa[indiceVencedor]->getNaipe());
            
            if (pesoNaipeAtual > pesoNaipeVencedor) {
                indiceVencedor = i;
            }
        }       
    } // fim do for

    return indiceVencedor;
}