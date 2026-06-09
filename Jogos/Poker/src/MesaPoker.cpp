#include "MesaPoker.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
int valorCarta(const Carta& carta) {
    int valor = static_cast<int>(carta.mostraValor());
    return valor == 1 ? 14 : valor;
}
}

MesaPoker::MesaPoker(int quantidadeJogadores, ModoPoker modo)
    : baralho_(1), quantidadeJogadores_(modo == ModoPoker::ContraComputador ? 2 : quantidadeJogadores),
      rodada_(0), vencedorRodada_(-1), empates_(0), modo_(modo),
      fase_(FasePoker::AguardandoJogadores) {
    if (quantidadeJogadores_ < 2 || quantidadeJogadores_ > 4)
        throw std::invalid_argument("A MesaPoker deve possuir entre 2 e 4 jogadores");

    jogadores_.resize(quantidadeJogadores_);
    nomes_.resize(quantidadeJogadores_);
    pontos_.assign(quantidadeJogadores_, 0);
    confirmouTroca_.assign(quantidadeJogadores_, false);
    trocasPendentes_.resize(quantidadeJogadores_);
    quantidadeUltimaTroca_.assign(quantidadeJogadores_, 0);

    for (int i = 0; i < quantidadeJogadores_; i++)
        nomes_[i] = "Jogador " + std::to_string(i + 1);

    if (modo_ == ModoPoker::ContraComputador) nomes_[1] = "Computador";
}

void MesaPoker::iniciar() {
    rodada_ = 1;
    vencedorRodada_ = -1;
    empates_ = 0;
    std::fill(pontos_.begin(), pontos_.end(), 0);
    distribuirCartas();
    fase_ = FasePoker::EscolhendoTrocas;
}

bool MesaPoker::iniciarNovaRodada() {
    if (fase_ != FasePoker::Resultado) return false;
    rodada_++;
    vencedorRodada_ = -1;
    distribuirCartas();
    fase_ = FasePoker::EscolhendoTrocas;
    return true;
}

void MesaPoker::definirNomeJogador(int idJogador, const std::string& nome) {
    if (!jogadorValido(idJogador)) throw std::out_of_range("Jogador invalido");
    nomes_[idJogador] = nome.empty() ? "Jogador " + std::to_string(idJogador + 1) : nome;
}

void MesaPoker::distribuirCartas() {
    baralho_ = Baralho(1);
    baralho_.embaralhar();

    for (Poker& jogador : jogadores_) jogador.limparMao();

    for (int i = 0; i < 5; i++) {
        for (Poker& jogador : jogadores_) {
            if (baralho_.estaVazio())
                throw std::runtime_error("O baralho acabou durante a distribuicao");

            jogador.receberCarta(baralho_.retirarCarta());
        }
    }

    limparConfirmacoes();
}

void MesaPoker::limparConfirmacoes() {
    std::fill(confirmouTroca_.begin(), confirmouTroca_.end(), false);
    std::fill(quantidadeUltimaTroca_.begin(), quantidadeUltimaTroca_.end(), 0);
    for (auto& troca : trocasPendentes_) troca.clear();
}

bool MesaPoker::confirmarTroca(int idJogador, const std::vector<int>& indices) {
    if (!podeConfirmarTroca(idJogador) || !indicesTrocaValidos(indices)) return false;

    trocasPendentes_[idJogador] = indices;
    quantidadeUltimaTroca_[idJogador] = static_cast<int>(indices.size());
    confirmouTroca_[idJogador] = true;

    if (modo_ == ModoPoker::ContraComputador) realizarJogadaComputador();

    if (todosConfirmaram()) {
        executarTrocas();
        finalizarRodada();
    }

    return true;
}

bool MesaPoker::podeConfirmarTroca(int idJogador) const {
    if (!jogadorValido(idJogador)) return false;
    if (fase_ != FasePoker::EscolhendoTrocas) return false;
    if (confirmouTroca_[idJogador]) return false;
    if (modo_ == ModoPoker::ContraComputador && idJogador == 1) return false;
    return jogadores_[idJogador].tamanhoMao() == 5;
}

bool MesaPoker::indicesTrocaValidos(const std::vector<int>& indices) const {
    if (indices.size() > 3) return false;

    std::vector<int> copia = indices;
    std::sort(copia.begin(), copia.end());

    if (std::adjacent_find(copia.begin(), copia.end()) != copia.end()) return false;

    return std::all_of(copia.begin(), copia.end(), [](int indice) {
        return indice >= 0 && indice < 5;
    });
}

void MesaPoker::realizarJogadaComputador() {
    constexpr int idComputador = 1;

    if (modo_ != ModoPoker::ContraComputador ||
        !jogadorValido(idComputador) ||
        confirmouTroca_[idComputador]) return;

    trocasPendentes_[idComputador] = escolherTrocasComputador();
    quantidadeUltimaTroca_[idComputador] =
        static_cast<int>(trocasPendentes_[idComputador].size());
    confirmouTroca_[idComputador] = true;
}

std::vector<int> MesaPoker::escolherTrocasComputador() const {
    constexpr int idComputador = 1;

    if (!jogadorValido(idComputador)) return {};

    const Poker& computador = jogadores_[idComputador];
    const std::vector<Carta>& mao = computador.verMao();

    if (computador.avaliarMao() >= 4) return {};

    std::vector<int> frequencias(15, 0);
    for (const Carta& carta : mao) frequencias[valorCarta(carta)]++;

    std::vector<int> indices;
    for (int i = 0; i < static_cast<int>(mao.size()); i++)
        if (frequencias[valorCarta(mao[i])] == 1) indices.push_back(i);

    if (indices.size() == 5) {
        std::vector<std::pair<int, int>> cartasOrdenadas;

        for (int i = 0; i < static_cast<int>(mao.size()); i++)
            cartasOrdenadas.push_back({valorCarta(mao[i]), i});

        std::sort(cartasOrdenadas.begin(), cartasOrdenadas.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });

        indices.clear();
        for (int i = 0; i < 3; i++) indices.push_back(cartasOrdenadas[i].second);
    }

    if (indices.size() > 3) indices.resize(3);

    return indices;
}

bool MesaPoker::todosConfirmaram() const {
    return std::all_of(confirmouTroca_.begin(), confirmouTroca_.end(),
        [](bool confirmou) { return confirmou; });
}

void MesaPoker::executarTrocas() {
    int totalNovasCartas = 0;
    for (const auto& troca : trocasPendentes_)
        totalNovasCartas += static_cast<int>(troca.size());

    if (baralho_.tamanho() < totalNovasCartas)
        throw std::runtime_error("Nao ha cartas suficientes para realizar as trocas");

    std::vector<std::vector<Carta>> novasCartas(quantidadeJogadores_);

    for (int id = 0; id < quantidadeJogadores_; id++) {
        novasCartas[id].reserve(trocasPendentes_[id].size());

        for (std::size_t i = 0; i < trocasPendentes_[id].size(); i++)
            novasCartas[id].push_back(baralho_.retirarCarta());
    }

    for (int id = 0; id < quantidadeJogadores_; id++) {
        if (!jogadores_[id].substituirCartas(trocasPendentes_[id], novasCartas[id]))
            throw std::runtime_error("Falha ao substituir cartas do jogador");
    }
}

void MesaPoker::finalizarRodada() {
    if (fase_ != FasePoker::EscolhendoTrocas || !todosConfirmaram()) return;

    int melhorJogador = 0;
    std::vector<int> empatados = {0};

    for (int i = 1; i < quantidadeJogadores_; i++) {
        int comparacao = jogadores_[i].compararCom(jogadores_[melhorJogador]);

        if (comparacao > 0) {
            melhorJogador = i;
            empatados = {i};
        } else if (comparacao == 0) {
            empatados.push_back(i);
        }
    }

    if (empatados.size() == 1) {
        vencedorRodada_ = melhorJogador;
        pontos_[melhorJogador]++;
    } else {
        vencedorRodada_ = -1;
        empates_++;
    }

    fase_ = FasePoker::Resultado;
}

bool MesaPoker::jogadorValido(int idJogador) const {
    return idJogador >= 0 && idJogador < quantidadeJogadores_;
}

const Poker& MesaPoker::jogador(int idJogador) const {
    if (!jogadorValido(idJogador)) throw std::out_of_range("Jogador invalido");
    return jogadores_[idJogador];
}

const std::vector<int>& MesaPoker::pontos() const {
    return pontos_;
}

const std::vector<int>& MesaPoker::quantidadeUltimaTroca() const {
    return quantidadeUltimaTroca_;
}

const std::string& MesaPoker::nomeJogador(int idJogador) const {
    if (!jogadorValido(idJogador)) throw std::out_of_range("Jogador invalido");
    return nomes_[idJogador];
}

bool MesaPoker::jogadorConfirmouTroca(int idJogador) const {
    return jogadorValido(idJogador) && confirmouTroca_[idJogador];
}

int MesaPoker::quantidadeJogadores() const {
    return quantidadeJogadores_;
}

int MesaPoker::rodada() const {
    return rodada_;
}

int MesaPoker::vencedorRodada() const {
    return vencedorRodada_;
}

int MesaPoker::empates() const {
    return empates_;
}

ModoPoker MesaPoker::modo() const {
    return modo_;
}

FasePoker MesaPoker::fase() const {
    return fase_;
}