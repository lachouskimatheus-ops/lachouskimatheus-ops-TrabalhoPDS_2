#include "pontuacao.h"
#include <fstream>
using namespace std;

Pontuacao::Pontuacao() : pontos(0), passadasCava(0) {
    carregarRecord();
}

void Pontuacao::carregarRecord() {
    std::ifstream arquivo(ARQUIVO_RECORD);
    if (arquivo.is_open()) {
        arquivo >> record;
    }
    
}
bool Pontuacao::salvarRecord() {
    if (pontos <= record)
        return false;
        record = pontos;
        ofstream arquivo(ARQUIVO_RECORD);
        if (arquivo.is_open())
        {
            arquivo<<record;
            return true;
        }
         return false;
        
    }

    int Pontuacao::getRecord() const{
        return record;
    }
void Pontuacao::aplicar(EventoPontuacao evento) {
    switch (evento) {
        case EventoPontuacao::CavaParaColuna:    
            pontos += 5;  
         break;

        case EventoPontuacao::CavaParaFundacao:   
            pontos += 10;  
         break;

        case EventoPontuacao::ColunaParaFundacao:  
            pontos += 10;  
        break;

        case EventoPontuacao::FundacaoParaColuna:  
            pontos -= 15;  
        break;
        
        case EventoPontuacao::VirarCarta:          
            pontos += 5;   
        break;

        case EventoPontuacao::PassarBaralho:
            if (passadasCava > 1) pontos -= 100;
            break;
    }
    if (pontos < 0) pontos = 0;
}

int Pontuacao::getPontos()const { 
return pontos; 
}
int Pontuacao::getPassadasCava() const { 
    return passadasCava; 
}
void Pontuacao::setPassadasCava(int p) {
     passadasCava = p; 
}
void Pontuacao::setPontos(int p){
     pontos = p; 
}
void Pontuacao::resetar(){
 pontos = 0; passadasCava = 0; 
} 