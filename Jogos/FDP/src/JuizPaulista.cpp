#include "JuizPaulista.hpp"

int JuizPaulista::decidirVencedor(std::vector<Carta*> cartasNaMesa, Carta vira, bool forcarVencedor) {

    int indiceVencedor = -1;
    int maiorForcaTotal = -1;
    bool empate = false;

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
        else{
            forcaDaCarta = cartasNaMesa[i]->forca();//Forca Normal
        }

        //Comparacao Forcas
        if(forcaDaCarta > maiorForcaTotal) {
            maiorForcaTotal = forcaDaCarta;
            indiceVencedor = i;
            empate = false;
        }else if (forcaDaCarta == maiorForcaTotal){
            if(forcarVencedor){//Caso empate 2 primeiras
                if(cartasNaMesa[i]->getForcaNaipe() > cartasNaMesa[indiceVencedor]->getForcaNaipe()){
                    indiceVencedor = i;
                    empate = false;
                }
            }else {
                empate = true;
            }
        }       
    }
if(empate == true) return -1;
return indiceVencedor;
}