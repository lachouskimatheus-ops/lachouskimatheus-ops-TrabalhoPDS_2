#include "Juiz.hpp"

int Juiz::decidirVencedor(std::vector<Carta> cartasNaMesa){
    int maiorForca = -1;
    int indiceVencedor = -1;
    bool empate = false;

    for(int i = 0; i < cartasNaMesa.size(); i++){
        
        int forcaAtual = cartasNaMesa[i].forca();
        if (forcaAtual > maiorForca){
            maiorForca = forcaAtual;
            indiceVencedor = i;
            empate = false;
        }
        else if(forcaAtual == maiorForca){
            empate = true;
        }
    }
    
    if (empate){return -1;}
    return indiceVencedor;
}

