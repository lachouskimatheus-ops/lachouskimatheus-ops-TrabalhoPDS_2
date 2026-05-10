#include "regrasPife.h"

bool indiceValido(const std::vector<Carta>& mao, int indice){
    if(indice < 0) return false;
    if(indice >= mao.size()) return false;
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

