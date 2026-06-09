#include "Poker.hpp"

#include <algorithm>
#include <map>
#include <stdexcept>
#include <vector>

namespace {
int valorNumerico(const Carta& carta) {
    int valor = static_cast<int>(carta.mostraValor());
    return valor == 1 ? 14 : valor;
}

std::vector<int> valoresOrdenadosDecrescente(const std::vector<Carta>& mao) {
    std::vector<int> valores;

    for (const Carta& carta : mao) valores.push_back(valorNumerico(carta));

    std::sort(valores.rbegin(), valores.rend());

    return valores;
}

std::map<int, int> frequenciasValores(const std::vector<Carta>& mao) {
    std::map<int, int> frequencias;

    for (const Carta& carta : mao) frequencias[valorNumerico(carta)]++;

    return frequencias;
}

bool ehFlush(const std::vector<Carta>& mao) {
    if (mao.size() != 5) return false;

    const auto naipe = mao.front().mostraNaipe();

    return std::all_of(
        mao.begin(),
        mao.end(),
        [naipe](const Carta& carta) {
            return carta.mostraNaipe() == naipe;
        }
    );
}

int maiorCartaSequencia(const std::vector<Carta>& mao) {
    if (mao.size() != 5) return 0;

    std::vector<int> valores = valoresOrdenadosDecrescente(mao);

    valores.erase(
        std::unique(valores.begin(), valores.end()),
        valores.end()
    );

    if (valores.size() != 5) return 0;

    if (
        valores[0] == 14 &&
        valores[1] == 5 &&
        valores[2] == 4 &&
        valores[3] == 3 &&
        valores[4] == 2
    ) {
        return 5;
    }

    for (std::size_t i = 1; i < valores.size(); i++) {
        if (valores[i - 1] - 1 != valores[i]) return 0;
    }

    return valores.front();
}
}

bool Poker::receberCarta(const Carta& carta) {
    if (mao_.size() >= 5) return false;

    mao_.push_back(carta);

    return true;
}

void Poker::limparMao() {
    mao_.clear();
}

bool Poker::substituirCartas(
    const std::vector<int>& indices,
    const std::vector<Carta>& novasCartas
) {
    if (mao_.size() != 5) return false;
    if (indices.size() != novasCartas.size()) return false;
    if (indices.size() > 3) return false;

    std::vector<int> copia = indices;
    std::sort(copia.begin(), copia.end());

    if (std::adjacent_find(copia.begin(), copia.end()) != copia.end()) {
        return false;
    }

    for (int indice : indices) {
        if (indice < 0 || indice >= static_cast<int>(mao_.size())) {
            return false;
        }
    }

    for (std::size_t i = 0; i < indices.size(); i++) {
        mao_[indices[i]] = novasCartas[i];
    }

    return true;
}

int Poker::avaliarMao() const {
    if (mao_.size() != 5) return -1;

    const bool flush = ehFlush(mao_);
    const int maiorSequencia = maiorCartaSequencia(mao_);
    const bool sequencia = maiorSequencia != 0;

    std::map<int, int> frequencias = frequenciasValores(mao_);

    int pares = 0;
    bool trinca = false;
    bool quadra = false;

    for (const auto& [valor, quantidade] : frequencias) {
        (void)valor;

        if (quantidade == 4) quadra = true;
        else if (quantidade == 3) trinca = true;
        else if (quantidade == 2) pares++;
    }

    if (flush && sequencia && maiorSequencia == 14) return 9;
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

std::vector<int> Poker::gerarPontuacaoDesempate() const {
    const int categoria = avaliarMao();

    if (categoria < 0) return {-1};

    std::vector<int> pontuacao = {categoria};
    std::map<int, int> frequencias = frequenciasValores(mao_);
    std::vector<int> valores = valoresOrdenadosDecrescente(mao_);

    if (categoria == 9) return pontuacao;

    if (categoria == 8 || categoria == 4) {
        pontuacao.push_back(maiorCartaSequencia(mao_));
        return pontuacao;
    }

    if (categoria == 7) {
        int quadra = 0;
        int kicker = 0;

        for (const auto& [valor, quantidade] : frequencias) {
            if (quantidade == 4) quadra = valor;
            else kicker = valor;
        }

        pontuacao.push_back(quadra);
        pontuacao.push_back(kicker);

        return pontuacao;
    }

    if (categoria == 6) {
        int trinca = 0;
        int par = 0;

        for (const auto& [valor, quantidade] : frequencias) {
            if (quantidade == 3) trinca = valor;
            else if (quantidade == 2) par = valor;
        }

        pontuacao.push_back(trinca);
        pontuacao.push_back(par);

        return pontuacao;
    }

    if (categoria == 3) {
        int trinca = 0;
        std::vector<int> kickers;

        for (const auto& [valor, quantidade] : frequencias) {
            if (quantidade == 3) trinca = valor;
            else kickers.push_back(valor);
        }

        std::sort(kickers.rbegin(), kickers.rend());

        pontuacao.push_back(trinca);
        pontuacao.insert(
            pontuacao.end(),
            kickers.begin(),
            kickers.end()
        );

        return pontuacao;
    }

    if (categoria == 2) {
        std::vector<int> pares;
        int kicker = 0;

        for (const auto& [valor, quantidade] : frequencias) {
            if (quantidade == 2) pares.push_back(valor);
            else kicker = valor;
        }

        std::sort(pares.rbegin(), pares.rend());

        pontuacao.push_back(pares[0]);
        pontuacao.push_back(pares[1]);
        pontuacao.push_back(kicker);

        return pontuacao;
    }

    if (categoria == 1) {
        int par = 0;
        std::vector<int> kickers;

        for (const auto& [valor, quantidade] : frequencias) {
            if (quantidade == 2) par = valor;
            else kickers.push_back(valor);
        }

        std::sort(kickers.rbegin(), kickers.rend());

        pontuacao.push_back(par);
        pontuacao.insert(
            pontuacao.end(),
            kickers.begin(),
            kickers.end()
        );

        return pontuacao;
    }

    pontuacao.insert(
        pontuacao.end(),
        valores.begin(),
        valores.end()
    );

    return pontuacao;
}

int Poker::compararCom(const Poker& outraMao) const {
    const std::vector<int> minhaPontuacao =
        gerarPontuacaoDesempate();

    const std::vector<int> outraPontuacao =
        outraMao.gerarPontuacaoDesempate();

    if (minhaPontuacao > outraPontuacao) return 1;
    if (minhaPontuacao < outraPontuacao) return -1;

    return 0;
}

std::string Poker::nomeDaJogada() const {
    switch (avaliarMao()) {
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
        default: return "Mao Invalida";
    }
}

int Poker::tamanhoMao() const {
    return static_cast<int>(mao_.size());
}

const std::vector<Carta>& Poker::verMao() const {
    return mao_;
}