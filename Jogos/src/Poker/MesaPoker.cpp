#include "MesaPoker.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

/**
 * @brief Converte o valor de uma carta para comparação no Poker.
 *
 * O Ás, representado internamente pelo valor 1, passa a valer 14 para ser
 * considerado superior ao Rei durante as avaliações auxiliares.
 *
 * @param carta Carta cujo valor será convertido.
 * @return Valor numérico da carta, considerando o Ás como 14.
 */
int valorCarta(const Carta& carta) {
    int valor = static_cast<int>(carta.mostraValor());
    return valor == 1 ? 14 : valor;
}

}

/**
 * @brief Constrói uma mesa de Poker.
 *
 * No modo contra o computador, a mesa sempre possui dois jogadores: um
 * jogador humano e um jogador controlado pelo computador.
 *
 * @param quantidadeJogadores Quantidade solicitada de jogadores.
 * @param modo Modo de funcionamento da partida.
 *
 * @throw std::invalid_argument Caso a quantidade de jogadores não esteja
 * entre 2 e 4.
 */
MesaPoker::MesaPoker(int quantidadeJogadores, ModoPoker modo)
    : baralho_(1),
      quantidadeJogadores_(modo == ModoPoker::ContraComputador ? 2 : quantidadeJogadores),
      rodada_(0),
      vencedorRodada_(-1),
      empates_(0),
      modo_(modo),
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

/**
 * @brief Inicializa uma nova partida.
 *
 * Reinicia o placar, o contador de empates e o estado da rodada, distribui
 * as cartas e inicia a fase de escolha das trocas.
 */
void MesaPoker::iniciar() {
    rodada_ = 1;
    vencedorRodada_ = -1;
    empates_ = 0;
    std::fill(pontos_.begin(), pontos_.end(), 0);

    distribuirCartas();
    fase_ = FasePoker::EscolhendoTrocas;
}

/**
 * @brief Inicia uma nova rodada após a apresentação do resultado anterior.
 *
 * @return true se a nova rodada foi iniciada; false se a mesa não estiver
 * na fase de resultado.
 */
bool MesaPoker::iniciarNovaRodada() {
    if (fase_ != FasePoker::Resultado) return false;

    rodada_++;
    vencedorRodada_ = -1;

    distribuirCartas();
    fase_ = FasePoker::EscolhendoTrocas;

    return true;
}

/**
 * @brief Define o nome de exibição de um jogador.
 *
 * Caso o nome informado esteja vazio, é atribuído novamente um nome padrão.
 *
 * @param idJogador Identificador do jogador.
 * @param nome Nome que será atribuído.
 *
 * @throw std::out_of_range Caso o identificador seja inválido.
 */
void MesaPoker::definirNomeJogador(int idJogador, const std::string& nome) {
    if (!jogadorValido(idJogador)) throw std::out_of_range("Jogador invalido");

    nomes_[idJogador] =
        nome.empty() ? "Jogador " + std::to_string(idJogador + 1) : nome;
}

/**
 * @brief Prepara o baralho e distribui cinco cartas para cada jogador.
 *
 * As mãos anteriores são apagadas e os estados relacionados às trocas são
 * reiniciados.
 *
 * @throw std::runtime_error Caso o baralho termine durante a distribuição.
 */
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

/**
 * @brief Reinicia os estados relacionados às trocas de cartas.
 */
void MesaPoker::limparConfirmacoes() {
    std::fill(confirmouTroca_.begin(), confirmouTroca_.end(), false);
    std::fill(quantidadeUltimaTroca_.begin(), quantidadeUltimaTroca_.end(), 0);

    for (auto& troca : trocasPendentes_) troca.clear();
}

/**
 * @brief Registra a escolha de troca de um jogador.
 *
 * Quando todos os jogadores confirmam suas escolhas, as trocas são executadas
 * e a rodada é finalizada.
 *
 * @param idJogador Identificador do jogador.
 * @param indices Índices das cartas que serão trocadas.
 * @return true se a confirmação foi aceita; false caso contrário.
 */
bool MesaPoker::confirmarTroca(int idJogador, const std::vector<int>& indices) {
    if (!podeConfirmarTroca(idJogador) || !indicesTrocaValidos(indices))
        return false;

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

/**
 * @brief Verifica se um jogador pode confirmar sua escolha de troca.
 *
 * @param idJogador Identificador do jogador.
 * @return true se o jogador pode confirmar; false caso contrário.
 */
bool MesaPoker::podeConfirmarTroca(int idJogador) const {
    if (!jogadorValido(idJogador)) return false;
    if (fase_ != FasePoker::EscolhendoTrocas) return false;
    if (confirmouTroca_[idJogador]) return false;
    if (modo_ == ModoPoker::ContraComputador && idJogador == 1) return false;

    return jogadores_[idJogador].tamanhoMao() == 5;
}

/**
 * @brief Verifica se os índices selecionados para troca são válidos.
 *
 * São permitidas no máximo três cartas, sem índices repetidos e apenas
 * posições entre 0 e 4.
 *
 * @param indices Índices selecionados.
 * @return true se todos os índices forem válidos; false caso contrário.
 */
bool MesaPoker::indicesTrocaValidos(const std::vector<int>& indices) const {
    if (indices.size() > 3) return false;

    std::vector<int> copia = indices;
    std::sort(copia.begin(), copia.end());

    if (std::adjacent_find(copia.begin(), copia.end()) != copia.end())
        return false;

    return std::all_of(copia.begin(), copia.end(), [](int indice) {
        return indice >= 0 && indice < 5;
    });
}

/**
 * @brief Executa a escolha automática de troca do computador.
 */
void MesaPoker::realizarJogadaComputador() {
    constexpr int idComputador = 1;

    if (modo_ != ModoPoker::ContraComputador ||
        !jogadorValido(idComputador) ||
        confirmouTroca_[idComputador])
        return;

    trocasPendentes_[idComputador] = escolherTrocasComputador();
    quantidadeUltimaTroca_[idComputador] =
        static_cast<int>(trocasPendentes_[idComputador].size());
    confirmouTroca_[idComputador] = true;
}

/**
 * @brief Escolhe as cartas que o computador deve trocar.
 *
 * O computador mantém mãos de categoria igual ou superior a sequência. Caso
 * existam pares ou trincas, troca apenas cartas sem repetição. Quando todas
 * as cartas têm valores diferentes, troca as três menores.
 *
 * @return Índices das cartas selecionadas para troca.
 */
std::vector<int> MesaPoker::escolherTrocasComputador() const {
    constexpr int idComputador = 1;

    if (!jogadorValido(idComputador)) return {};

    const Poker& computador = jogadores_[idComputador];
    const std::vector<Carta>& mao = computador.verMao();

    if (computador.avaliarMao() >= 4) return {};

    std::vector<int> frequencias(15, 0);

    for (const Carta& carta : mao)
        frequencias[valorCarta(carta)]++;

    std::vector<int> indices;

    for (int i = 0; i < static_cast<int>(mao.size()); i++) {
        if (frequencias[valorCarta(mao[i])] == 1)
            indices.push_back(i);
    }

    if (indices.size() == 5) {
        std::vector<std::pair<int, int>> cartasOrdenadas;

        for (int i = 0; i < static_cast<int>(mao.size()); i++)
            cartasOrdenadas.push_back({valorCarta(mao[i]), i});

        std::sort(
            cartasOrdenadas.begin(),
            cartasOrdenadas.end(),
            [](const auto& a, const auto& b) {
                return a.first < b.first;
            }
        );

        indices.clear();

        for (int i = 0; i < 3; i++)
            indices.push_back(cartasOrdenadas[i].second);
    }

    if (indices.size() > 3) indices.resize(3);

    return indices;
}

/**
 * @brief Verifica se todos os jogadores confirmaram suas trocas.
 *
 * @return true se todos confirmaram; false caso contrário.
 */
bool MesaPoker::todosConfirmaram() const {
    return std::all_of(
        confirmouTroca_.begin(),
        confirmouTroca_.end(),
        [](bool confirmou) {
            return confirmou;
        }
    );
}

/**
 * @brief Executa todas as trocas pendentes.
 *
 * Primeiro são retiradas do baralho todas as novas cartas necessárias.
 * Depois, as cartas são substituídas nas mãos dos jogadores.
 *
 * @throw std::runtime_error Caso não existam cartas suficientes ou alguma
 * substituição falhe.
 */
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
        if (!jogadores_[id].substituirCartas(
                trocasPendentes_[id],
                novasCartas[id]))
            throw std::runtime_error("Falha ao substituir cartas do jogador");
    }
}

/**
 * @brief Compara as mãos, determina o vencedor e atualiza o placar.
 */
void MesaPoker::finalizarRodada() {
    if (fase_ != FasePoker::EscolhendoTrocas || !todosConfirmaram())
        return;

    int melhorJogador = 0;
    std::vector<int> empatados = {0};

    for (int i = 1; i < quantidadeJogadores_; i++) {
        int comparacao =
            jogadores_[i].compararCom(jogadores_[melhorJogador]);

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

/**
 * @brief Verifica se o identificador corresponde a um jogador existente.
 *
 * @param idJogador Identificador do jogador.
 * @return true se o identificador for válido; false caso contrário.
 */
bool MesaPoker::jogadorValido(int idJogador) const {
    return idJogador >= 0 && idJogador < quantidadeJogadores_;
}

/**
 * @brief Retorna os dados de um jogador.
 *
 * @param idJogador Identificador do jogador.
 * @return Referência constante para o jogador.
 *
 * @throw std::out_of_range Caso o identificador seja inválido.
 */
const Poker& MesaPoker::jogador(int idJogador) const {
    if (!jogadorValido(idJogador))
        throw std::out_of_range("Jogador invalido");

    return jogadores_[idJogador];
}

/**
 * @brief Retorna o placar dos jogadores.
 *
 * @return Referência constante para o vetor de pontos.
 */
const std::vector<int>& MesaPoker::pontos() const {
    return pontos_;
}

/**
 * @brief Retorna a quantidade de cartas trocadas por cada jogador.
 *
 * @return Referência constante para o vetor de quantidades.
 */
const std::vector<int>& MesaPoker::quantidadeUltimaTroca() const {
    return quantidadeUltimaTroca_;
}

/**
 * @brief Retorna os nomes de todos os jogadores.
 *
 * @return Referência constante para o vetor de nomes.
 */
const std::vector<std::string>& MesaPoker::nomes() const {
    return nomes_;
}

/**
 * @brief Retorna o nome de um jogador.
 *
 * @param idJogador Identificador do jogador.
 * @return Referência constante para o nome.
 *
 * @throw std::out_of_range Caso o identificador seja inválido.
 */
const std::string& MesaPoker::nomeJogador(int idJogador) const {
    if (!jogadorValido(idJogador))
        throw std::out_of_range("Jogador invalido");

    return nomes_[idJogador];
}

/**
 * @brief Verifica se um jogador confirmou sua troca.
 *
 * @param idJogador Identificador do jogador.
 * @return true se o jogador for válido e já tiver confirmado; false caso contrário.
 */
bool MesaPoker::jogadorConfirmouTroca(int idJogador) const {
    return jogadorValido(idJogador) && confirmouTroca_[idJogador];
}

/**
 * @brief Retorna a quantidade de jogadores da mesa.
 *
 * @return Quantidade de jogadores.
 */
int MesaPoker::quantidadeJogadores() const {
    return quantidadeJogadores_;
}

/**
 * @brief Retorna o número da rodada atual.
 *
 * @return Número da rodada.
 */
int MesaPoker::rodada() const {
    return rodada_;
}

/**
 * @brief Retorna o identificador do vencedor da última rodada.
 *
 * @return Identificador do vencedor ou -1 em caso de empate.
 */
int MesaPoker::vencedorRodada() const {
    return vencedorRodada_;
}

/**
 * @brief Retorna a quantidade acumulada de empates.
 *
 * @return Quantidade de empates.
 */
int MesaPoker::empates() const {
    return empates_;
}

/**
 * @brief Retorna o modo atual da partida.
 *
 * @return Modo da partida.
 */
ModoPoker MesaPoker::modo() const {
    return modo_;
}

/**
 * @brief Retorna a fase atual da partida.
 *
 * @return Fase atual.
 */
FasePoker MesaPoker::fase() const {
    return fase_;
}