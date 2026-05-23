#include "poker.h"
#include <algorithm>

Poker::Poker() {
}

//converte o enum Valor para um número que facilita comparar as cartas
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

//adiciona uma carta na mão do jogador de poker
void Poker::receberCarta(const Carta& carta) {
    mao.push_back(carta);
}

//limpa a mão para iniciar outra rodada
void Poker::limparMao() {
    mao.clear();
}

int Poker::tamanhoMao() const {
    return mao.size();
}

const std::vector<Carta>& Poker::verMao() const {
    return mao;
}

//conta quantas cartas de cada valor existem na mão
std::vector<int> Poker::contarValores(const std::vector<Carta>& cartas) const {
    std::vector<int> frequencia(15, 0);

    for (int i = 0; i < cartas.size(); i++) {
        int valor = valorNumerico(cartas[i].mostraValor());
        if (valor >= 2 && valor <= 14) {
            frequencia[valor]++;
        }
    }

    return frequencia;
}

//verifica se todas as cartas têm o mesmo naipe
bool Poker::ehFlush(const std::vector<Carta>& cartas) const {
    if (cartas.size() != 5) return false;

    Naipe primeiroNaipe = cartas[0].mostraNaipe();

    for (int i = 1; i < cartas.size(); i++) {
        if (cartas[i].mostraNaipe() != primeiroNaipe) {
            return false;
        }
    }

    return true;
}

//verifica se os valores formam uma sequência
bool Poker::ehSequencia(const std::vector<Carta>& cartas) const {
    if (cartas.size() != 5) return false;

    std::vector<int> valores;

    for (int i = 0; i < cartas.size(); i++) {
        valores.push_back(valorNumerico(cartas[i].mostraValor()));
    }

    std::sort(valores.begin(), valores.end());

    //caso especial: A, 2, 3, 4, 5
    if (valores[0] == 2 && valores[1] == 3 && valores[2] == 4 &&
        valores[3] == 5 && valores[4] == 14) {
        return true;
    }

    for (int i = 1; i < valores.size(); i++) {
        if (valores[i] != valores[i - 1] + 1) {
            return false;
        }
    }

    return true;
}

//retorna a força da mão
//0 carta alta, 1 par, 2 dois pares, 3 trinca, 4 sequência,
//5 flush, 6 full house, 7 quadra, 8 straight flush, 9 royal flush
int Poker::avaliarMao() const {
    if (mao.size() != 5) return -1;

    std::vector<int> freq = contarValores(mao);

    bool flush = ehFlush(mao);
    bool sequencia = ehSequencia(mao);

    bool quadra = false;
    bool trinca = false;
    int pares = 0;

    for (int i = 2; i <= 14; i++) {
        if (freq[i] == 4) quadra = true;
        if (freq[i] == 3) trinca = true;
        if (freq[i] == 2) pares++;
    }

    std::vector<int> valores;
    for (int i = 0; i < mao.size(); i++) {
        valores.push_back(valorNumerico(mao[i].mostraValor()));
    }

    std::sort(valores.begin(), valores.end());

    if (flush && valores[0] == 10 && valores[1] == 11 &&
        valores[2] == 12 && valores[3] == 13 && valores[4] == 14) {
        return 9;
    }

    if (flush && sequencia) return 8;
    if (quadra) return 7;
    if (trinca && pares == 1) return 6;
    if (flush) return 5;
    if (sequencia) return 4;
    if (trinca) return 3;
    if (pares == 2) return 2;
    if (pares == 1) return 1;

    return 0;
}

//transforma a força da mão em texto para aparecer no jogo
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