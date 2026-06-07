#include "RegrasPife.hpp"

#include <algorithm>
#include <vector>


bool indiceValido(const std::vector<Carta>& mao, int indice) {
    return (
        indice >= 0 &&
        indice < static_cast<int>(mao.size())
    );
}


Valor valorDoCoringa(const Carta& vira) {
    Valor valorVira = vira.mostraValor();

    if (valorVira == Valor::Rei) {
        return Valor::As;
    }

    return static_cast<Valor>(
        static_cast<int>(valorVira) + 1
    );
}


bool cartaEhCoringa(
    const Carta& carta,
    const Carta& vira
) {
    return (
        carta.mostraValor() == valorDoCoringa(vira) &&
        carta.mostraNaipe() == vira.mostraNaipe()
    );
}

bool trinca(const std::vector<Carta>& mao, int i1, int i2, int i3, const Carta& vira) {
    if (!indiceValido(mao, i1)) {
        return false;
    }

    if (!indiceValido(mao, i2)) {
        return false;
    }

    if (!indiceValido(mao, i3)) {
        return false;
    }

    std::vector<Carta> cartas = {
        mao[i1],
        mao[i2],
        mao[i3]
    };

    std::vector<Carta> cartasNormais;

    int quantidadeCoringas = 0;

    for (const Carta& carta : cartas) {
        if (cartaEhCoringa(carta, vira)) {
            quantidadeCoringas++;
        } else {
            cartasNormais.push_back(carta);
        }
    }

    if (quantidadeCoringas == 3) {
        return true;
    }

    Valor valorTrinca =
        cartasNormais.front().mostraValor();

    for (const Carta& carta : cartasNormais) {
        if (carta.mostraValor() != valorTrinca) {
            return false;
        }
    }

    for (int i = 0; i < static_cast<int>(cartasNormais.size()); i++){
        for (int j = i + 1; j < static_cast<int>(cartasNormais.size()); j++){
            if (cartasNormais[i].mostraNaipe() == cartasNormais[j].mostraNaipe()) {
                return false;
            }
        }
    }

    return true;
}


bool sequencia( const std::vector<Carta>& mao, int i1, int i2, int i3, const Carta& vira) {
    if (!indiceValido(mao, i1)) {
        return false;
    }

    if (!indiceValido(mao, i2)) {
        return false;
    }

    if (!indiceValido(mao, i3)) {
        return false;
    }

    std::vector<Carta> cartas = {mao[i1],mao[i2], mao[i3]};
    std::vector<Carta> cartasNormais;

    int quantidadeCoringas = 0;

    for (const Carta& carta : cartas) {
        if (cartaEhCoringa(carta, vira)) {
            quantidadeCoringas++;
        } else {
            cartasNormais.push_back(carta);
        }
    }

    if (quantidadeCoringas == 3) {
        return true;
    }

    Naipe naipeSequencia = cartasNormais.front().mostraNaipe();

    for (const Carta& carta : cartasNormais) {
        if (carta.mostraNaipe() != naipeSequencia) {
            return false;
        }
    }

    std::vector<int> valores;

    for (const Carta& carta : cartasNormais) {
        valores.push_back(
            static_cast<int>(carta.mostraValor())
        );
    }

    std::sort(valores.begin(), valores.end()); //organiza os valores das cartas em ordem crescente.

    for (int i = 1; i < static_cast<int>(valores.size()); i++) {
        if (valores[i] == valores[i - 1]) {
            return false;
        }
    }

    const int valorAs = static_cast<int>(Valor::As);

    const int valorRei = static_cast<int>(Valor::Rei);

//testa as sequencias possiveis
    for (int inicio = valorAs; inicio <= valorRei - 2; inicio++) {
        bool cartasCabemNaSequencia = true;
        for (int valor : valores) {
            if (valor < inicio || valor > inicio + 2) {
                cartasCabemNaSequencia = false;
                break;
            }
        }
        if (cartasCabemNaSequencia) {
            return true;
        }
    }
    return false;
}


bool combinacaoValida( const std::vector<Carta>& mao, int i1, int i2, int i3, const Carta& vira) {
    return trinca(mao, i1, i2, i3, vira) || sequencia(mao, i1, i2, i3, vira);
}

bool buscaCombinacao(std::vector<Carta> mao, int combinacoesFormadas, const Carta& vira) {
    if (combinacoesFormadas == 3) {
        return mao.empty() || mao.size() == 1;
    }

    if (mao.size() < 3) {
        return false;
    }
    for (int i = 0; i < static_cast<int>(mao.size()); i++) {
        for (int j = i + 1; j < static_cast<int>(mao.size()); j++){
            for (int k = j + 1; k < static_cast<int>(mao.size()); k++) {
                if(!combinacaoValida(mao, i, j, k, vira)) {
                    continue;
                }
                std::vector<Carta> resto;
                resto.reserve(mao.size() - 3);
                for (int indice = 0; indice < static_cast<int>(mao.size()); indice++) {
                    if (indice != i && indice != j && indice != k) {
                        resto.push_back(mao[indice]);
                    }
                }
                if (buscaCombinacao(resto, combinacoesFormadas + 1, vira)) {
                    return true;
                }
            }
        }
    }

    return false;
}


bool verificaVitoria(const std::vector<Carta>& mao, const Carta& vira) {
    if (mao.size() < 9 || mao.size() > 10) {
        return false;
    }
    return buscaCombinacao(mao, 0, vira);
}