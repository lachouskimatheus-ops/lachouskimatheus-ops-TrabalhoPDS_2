#include "JuizMineiro.hpp"

int JuizMineiro::decidirVencedor(std::vector<Carta> cartasNaMesa, Carta vira, bool forcarVencedor) {

    int indiceVencedor = -1;
    int maiorForcaTotal = -1;
    bool empate = false;

    for (int i=0; i < cartasNaMesa.size(); i++){
        int forcaDaCarta;
        int v = cartasNaMesa[i].getValor();
        Naipe n = cartasNaMesa[i].getNaipe();

        //Forca Manilhas
        if (v == 4 && n == Naipe::paus)          {forcaDaCarta = 100;}
        else if (v == 7 && n == Naipe::copas)    {forcaDaCarta = 99;}
        else if (v == 1 && n == Naipe::espadas)  {forcaDaCarta = 98;}
        else if (v == 7 && n == Naipe::ouros)    {forcaDaCarta = 97;}
        else{
            forcaDaCarta = cartasNaMesa[i].forca();//Forca Normal
        }

        //Comparacao Forcas
        if(forcaDaCarta > maiorForcaTotal) {
            maiorForcaTotal = forcaDaCarta;
            indiceVencedor = i;
            empate = false;
        }else if (forcaDaCarta == maiorForcaTotal){
            if(forcarVencedor){//Caso empate 2 primeiras
                if(cartasNaMesa[i].getForcaNaipe() > cartasNaMesa[indiceVencedor].getForcaNaipe()){
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