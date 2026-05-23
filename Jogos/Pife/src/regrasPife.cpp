#include "regrasPife.h"

bool indiceValido(const std::vector<Carta>& mao, int indice){
    if(indice < 0) return false;
    if(indice >= static_cast<int>(mao.size())) return false;
    return true;
}

bool trinca(const std::vector<Carta>& mao, int i1, int i2, int i3){
    if(!indiceValido(mao, i1)) return false;
    if(!indiceValido(mao, i2)) return false;
    if(!indiceValido(mao, i3)) return false;

    Valor v1 = mao[i1].mostraValor();
    Valor v2 = mao[i2].mostraValor();
    Valor v3 = mao[i3].mostraValor();

    Naipe n1 = mao[i1].mostraNaipe();
    Naipe n2 = mao[i2].mostraNaipe();
    Naipe n3 = mao[i3].mostraNaipe();

    return (v1==v2 && v2==v3)&& (n1 != n2) && (n1 != n3) && (n3 != n2);
}

bool sequencia(const std::vector<Carta>& mao, int i1, int i2, int i3){
    if(!indiceValido(mao, i1)) return false;
    if(!indiceValido(mao, i2)) return false;
    if(!indiceValido(mao, i3)) return false;

    int v1 = static_cast<int>(mao[i1].mostraValor());
    int v2 = static_cast<int>(mao[i2].mostraValor());
    int v3 = static_cast<int>(mao[i3].mostraValor());
    
    Naipe n1 = mao[i1].mostraNaipe();
    Naipe n2 = mao[i2].mostraNaipe();
    Naipe n3 = mao[i3].mostraNaipe();
    
    return ((v1 + 1)==v2 && (v2 + 1)==v3) && (n1 == n2 && n2 == n3);
}

bool combinacaoValida(const std::vector<Carta>& mao, int i1, int i2, int i3){
    return trinca(mao, i1, i2, i3) || sequencia(mao, i1, i2, i3);
}


/*função que mais sofri pra fazer, tentei fazer ela de outras formas, mas não tava atendendo os requisitos,
fui pesquisar formas e vi essa de testar todas as combinações, usando recursão para não ter for infinitos.
Sofri porque tinha que fazer a validação para 10 ou 9 cartas na mão, no inicio era usar o organizarMao() para ajudar na validação,
 e validar em sequencia, mas com 10 cartas isso daria erro*/ 
bool buscaCombinacao(std::vector<Carta> mao, int paresFormados){

    if(paresFormados == 3){ //retorna true se tivar os 3 pares e de tiver 1 ou 0 cartas na cópia da mao
        return mao.size() == 0 || mao.size() == 1;
    }

    if(mao.size() <3){ //false se tiver menos de 3 cartas e sem as 3 combinações validas
        return false;
    }

    for(int i = 0; i < mao.size(); i++){//lógica para ler todas as compinações
        for(int j = i + 1; j < mao.size(); j++){
            for(int k = j+1; k < mao.size(); k++){

                if(combinacaoValida(mao, i, j, k)){ //se a combinção for válida, cria um vetor temporario para uma mão sem a combinação
                    std::vector<Carta> resto;
                

                    for(int c = 0;c < mao.size(); c++){ //coloca o resto no vetor
                        if (c != i && c != j && c != k){
                            resto.push_back(mao[c]);
                        }
                    }

                    if(buscaCombinacao(resto, paresFormados + 1)){ //agora testa para o resto
                        return true;
                    }
            
                }
            }
        }

    }
    
    return false;
}

bool verificaVitoria(const std::vector<Carta>& mao){
    if(mao.size() <9 || mao.size() > 10){
        return false;
    }
    return buscaCombinacao(mao, 0);
}

