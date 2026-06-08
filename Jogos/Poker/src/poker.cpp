#include "poker.h"
#include "Baralho.hpp"

#include <algorithm>
#include <functional>

// Construtor
Poker::Poker() {}

// Converte o enum Valor para um número.
// No Poker, o Ás é considerado a carta mais forte, valendo 14.
int Poker::valorNumerico(Valor valor) const {
    switch (valor) {
        case Valor::As: return 14;
        case Valor::Dois: return 2;
        case Valor::Tres: return 3;
        case Valor::Quatro: return 4;
        case Valor::Cinco: return 5;
        case Valor::Seis: return 6;
        case Valor::Sete: return 7;
        case Valor::Oito: return 8;
        case Valor::Nove: return 9;
        case Valor::Dez: return 10;
        case Valor::Valete: return 11;
        case Valor::Dama: return 12;
        case Valor::Rei: return 13;
        default: return 0;
    }
}

// Adiciona uma carta na mão do jogador.
// A mão do Poker possui no máximo 5 cartas.
void Poker::receberCarta(const Carta& carta) {
    if (mao.size() < 5) {
        mao.push_back(carta);
    }
}

// Limpa a mão para iniciar outra rodada.
void Poker::limparMao() {
    mao.clear();
}

// Retorna a quantidade de cartas na mão.
int Poker::tamanhoMao() const {
    return static_cast<int>(mao.size());
}

// Retorna a mão atual.
const std::vector<Carta>& Poker::verMao() const {
    return mao;
}

// Conta quantas cartas de cada valor existem na mão.
std::vector<int> Poker::contarValores(const std::vector<Carta>& cartas) const {
    std::vector<int> frequencia(15, 0);

    for (size_t i = 0; i < cartas.size(); i++) {
        int valor = valorNumerico(cartas[i].mostraValor());

        if (valor >= 2 && valor <= 14) {
            frequencia[valor]++;
        }
    }

    return frequencia;
}

// Verifica se todas as cartas têm o mesmo naipe.
bool Poker::ehFlush(const std::vector<Carta>& cartas) const {
    if (cartas.size() != 5) {
        return false;
    }

    Naipe primeiroNaipe = cartas[0].mostraNaipe();

    for (size_t i = 1; i < cartas.size(); i++) {
        if (cartas[i].mostraNaipe() != primeiroNaipe) {
            return false;
        }
    }

    return true;
}

// Verifica se os valores formam uma sequência.
// Também trata o caso especial A, 2, 3, 4, 5.
bool Poker::ehSequencia(const std::vector<Carta>& cartas) const {
    if (cartas.size() != 5) {
        return false;
    }

    std::vector<int> valores;

    for (size_t i = 0; i < cartas.size(); i++) {
        valores.push_back(valorNumerico(cartas[i].mostraValor()));
    }

    std::sort(valores.begin(), valores.end());

    // Caso especial: A, 2, 3, 4, 5
    if (valores[0] == 2 &&
        valores[1] == 3 &&
        valores[2] == 4 &&
        valores[3] == 5 &&
        valores[4] == 14) {
        return true;
    }

    for (size_t i = 1; i < valores.size(); i++) {
        if (valores[i] != valores[i - 1] + 1) {
            return false;
        }
    }

    return true;
}

// Retorna a força da mão.
// 0 = Carta Alta
// 1 = Um Par
// 2 = Dois Pares
// 3 = Trinca
// 4 = Sequência
// 5 = Flush
// 6 = Full House
// 7 = Quadra
// 8 = Straight Flush
// 9 = Royal Flush
int Poker::avaliarMao() const {
    if (mao.size() != 5) {
        return -1;
    }

    std::vector<int> freq = contarValores(mao);

    bool flush = ehFlush(mao);
    bool sequencia = ehSequencia(mao);

    bool quadra = false;
    bool trinca = false;
    int pares = 0;

    for (int i = 2; i <= 14; i++) {
        if (freq[i] == 4) {
            quadra = true;
        }

        if (freq[i] == 3) {
            trinca = true;
        }

        if (freq[i] == 2) {
            pares++;
        }
    }

    std::vector<int> valores;

    for (size_t i = 0; i < mao.size(); i++) {
        valores.push_back(valorNumerico(mao[i].mostraValor()));
    }

    std::sort(valores.begin(), valores.end());

    if (flush &&
        valores[0] == 10 &&
        valores[1] == 11 &&
        valores[2] == 12 &&
        valores[3] == 13 &&
        valores[4] == 14) {
        return 9;
    }

    if (flush && sequencia) {
        return 8;
    }

    if (quadra) {
        return 7;
    }

    if (trinca && pares == 1) {
        return 6;
    }

    if (flush) {
        return 5;
    }

    if (sequencia) {
        return 4;
    }

    if (trinca) {
        return 3;
    }

    if (pares == 2) {
        return 2;
    }

    if (pares == 1) {
        return 1;
    }

    return 0;
}

// Transforma a força da mão em texto para aparecer no jogo.
std::string Poker::nomeJogada() const {
    int resultado = avaliarMao();

    switch (resultado) {
        case 9: return "Royal Flush";
        case 8: return "Straight Flush";
        case 7: return "Quadra";
        case 6: return "Full House";
        case 5: return "Flush";
        case 4: return "Sequencia";
        case 3: return "Trinca";
        case 2: return "Dois Pares";
        case 1: return "Um Par";
        case 0: return "Carta Alta";
        default: return "Mao invalida";
    }
}

// Gera uma lista de números usada para desempatar mãos.
// O primeiro número é a categoria da mão.
// Os próximos números representam os critérios de desempate.
std::vector<int> Poker::gerarPontuacaoDesempate() const {
    std::vector<int> resultado;

    int categoria = avaliarMao();
    resultado.push_back(categoria);

    if (categoria == -1) {
        return resultado;
    }

    std::vector<int> freq = contarValores(mao);

    std::vector<int> quadras;
    std::vector<int> trincas;
    std::vector<int> pares;
    std::vector<int> avulsas;

    for (int valor = 14; valor >= 2; valor--) {
        if (freq[valor] == 4) {
            quadras.push_back(valor);
        } else if (freq[valor] == 3) {
            trincas.push_back(valor);
        } else if (freq[valor] == 2) {
            pares.push_back(valor);
        } else if (freq[valor] == 1) {
            avulsas.push_back(valor);
        }
    }

    if (categoria == 9 || categoria == 8 || categoria == 4) {
        std::vector<int> valores;

        for (size_t i = 0; i < mao.size(); i++) {
            valores.push_back(valorNumerico(mao[i].mostraValor()));
        }

        std::sort(valores.begin(), valores.end(), std::greater<int>());

        if (valores.size() == 5 &&
            valores[0] == 14 &&
            valores[1] == 5 &&
            valores[2] == 4 &&
            valores[3] == 3 &&
            valores[4] == 2) {
            resultado.push_back(5);
        } else {
            resultado.push_back(valores[0]);
        }
    } else if (categoria == 7) {
        resultado.push_back(quadras[0]);
        resultado.push_back(avulsas[0]);
    } else if (categoria == 6) {
        resultado.push_back(trincas[0]);
        resultado.push_back(pares[0]);
    } else if (categoria == 5 || categoria == 0) {
        std::vector<int> valores;

        for (size_t i = 0; i < mao.size(); i++) {
            valores.push_back(valorNumerico(mao[i].mostraValor()));
        }

        std::sort(valores.begin(), valores.end(), std::greater<int>());

        for (int v : valores) {
            resultado.push_back(v);
        }
    } else if (categoria == 3) {
        resultado.push_back(trincas[0]);

        for (int v : avulsas) {
            resultado.push_back(v);
        }
    } else if (categoria == 2) {
        for (int v : pares) {
            resultado.push_back(v);
        }

        resultado.push_back(avulsas[0]);
    } else if (categoria == 1) {
        resultado.push_back(pares[0]);

        for (int v : avulsas) {
            resultado.push_back(v);
        }
    }

    return resultado;
}

// Compara a mão atual com a mão de outro jogador.
// Retorna:
//  1 se esta mão vence
// -1 se a outra mão vence
//  0 se empata
int Poker::compararCom(const Poker& outro) const {
    std::vector<int> p1 = gerarPontuacaoDesempate();
    std::vector<int> p2 = outro.gerarPontuacaoDesempate();

    size_t tamanho = std::min(p1.size(), p2.size());

    for (size_t i = 0; i < tamanho; i++) {
        if (p1[i] > p2[i]) {
            return 1;
        }

        if (p1[i] < p2[i]) {
            return -1;
        }
    }

    return 0;
}

// Troca cartas da mão do jogador.
// Recebe os índices das cartas escolhidas e compra novas cartas do baralho.
// Permite trocar no máximo 3 cartas.
bool Poker::trocarCartas(const std::vector<int>& indices, Baralho& baralho) {
    if (mao.size() != 5) {
        return false;
    }

    if (indices.size() > 3) {
        return false;
    }

    std::vector<int> indicesValidos;

    for (int indice : indices) {
        if (indice < 0 || indice >= 5) {
            return false;
        }

        if (std::find(indicesValidos.begin(), indicesValidos.end(), indice) == indicesValidos.end()) {
            indicesValidos.push_back(indice);
        }
    }

    if (indicesValidos.size() > 3) {
        return false;
    }

    for (int indice : indicesValidos) {
        if (baralho.estaVazio()) {
            return false;
        }

        mao[indice] = baralho.retirarCarta();
    }

    return true;
}
