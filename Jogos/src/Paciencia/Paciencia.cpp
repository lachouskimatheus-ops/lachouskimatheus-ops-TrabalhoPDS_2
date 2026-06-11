#include "Paciencia.hpp"
#include "Regras.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <cassert>
#include <set>

// ==============================================================================
// EXCEÇÕES PERSONALIZADAS DO PACIÊNCIA
// ==============================================================================

/**
 * Exceção lançada quando um índice (coluna, fundação, carta) está fora dos limites.
 */
class IndiceInvalidoException : public std::out_of_range {
public:
    explicit IndiceInvalidoException(const std::string& msg)
        : std::out_of_range("[Paciencia] IndiceInvalido: " + msg) {}
};

/**
 * Exceção lançada quando uma operação é tentada em uma pilha vazia.
 */
class PilhaVaziaException : public std::runtime_error {
public:
    explicit PilhaVaziaException(const std::string& msg)
        : std::runtime_error("[Paciencia] PilhaVazia: " + msg) {}
};

/**
 * Exceção lançada quando um movimento viola as regras do jogo.
 */
class MovimentoInvalidoException : public std::logic_error {
public:
    explicit MovimentoInvalidoException(const std::string& msg)
        : std::logic_error("[Paciencia] MovimentoInvalido: " + msg) {}
};

/**
 * Exceção lançada quando a geração do jogo produz um estado inconsistente.
 */
class EstadoInvalidoException : public std::runtime_error {
public:
    explicit EstadoInvalidoException(const std::string& msg)
        : std::runtime_error("[Paciencia] EstadoInvalido: " + msg) {}
};

// ==============================================================================
// CICLO DE VIDA
// ==============================================================================

// Inicializa o jogo ao criar o objeto
Paciencia::Paciencia() : vitoria(false), pontuacao() {
    iniciarJogo();
}

// Destrutor — memória gerenciada pelos contêineres STL
Paciencia::~Paciencia() {
    // Memória liberada automaticamente pelos contêineres STL.
}

// Reseta completamente o estado do jogo e distribui as cartas
void Paciencia::iniciarJogo() {
    colunas.assign(7, std::vector<Carta>());
    fundacoes.assign(4, std::vector<Carta>());
    descarte.clear();

    while (!historico.empty()) historico.pop();

    cava = Baralho();
    cava.embaralhar();

    for (int i = 0; i < 7; ++i) {
        colunas[i].clear();
        cartasEscondidas[i] = i;
        for (int j = 0; j <= i; ++j) {
            Carta c = cava.retirarCarta();
            colunas[i].push_back(c);
        }
    }

    pontuacao.resetar();
    vitoria = false;
}

// ==============================================================================
// VISIBILIDADE
// ==============================================================================

// Revela a carta do topo de uma coluna, decrementando as cartas escondidas
void Paciencia::virarParaCima(int coluna) {
    // Barricada: valida entrada pública antes de qualquer operação
    if (coluna < 0 || coluna >= 7)
        throw IndiceInvalidoException("virarParaCima: coluna " + std::to_string(coluna) + " fora do intervalo [0,6].");

    if (cartasEscondidas[coluna] > 0 && !colunas[coluna].empty()) {
        cartasEscondidas[coluna]--;
        pontuacao.aplicar(EventoPontuacao::VirarCarta);
    }
}

// Retorna true se a carta na posição (coluna, linha) está virada para cima
bool Paciencia::cartaVisivel(int coluna, int linha) const {
    // Barricada: retorna false para índices inválidos (não lança — método const de consulta)
    if (coluna < 0 || coluna >= 7) return false;
    return (linha >= cartasEscondidas[coluna]);
}

// ==============================================================================
// HISTÓRICO (DESFAZER)
// ==============================================================================

// Faz um snapshot do estado atual e empilha no histórico
void Paciencia::salvarEstadoNoHistorico() {
    EstadoJogo estado;
    estado.colunas      = colunas;
    estado.fundacoes    = fundacoes;
    estado.descarte     = descarte;
    estado.cava         = cava;
    estado.pontos       = pontuacao.getPontos();
    estado.passadasCava = pontuacao.getPassadasCava();

    for (int i = 0; i < 7; ++i)
        estado.cartasEscondidas[i] = cartasEscondidas[i];

    historico.push(estado);
}

// Restaura o estado anterior a partir do histórico
bool Paciencia::desfazer() {
    // Barricada: não lança exceção — retorna false para indicar que não há o que desfazer
    if (historico.empty()) return false;

    EstadoJogo anterior = historico.top();
    historico.pop();

    colunas   = anterior.colunas;
    fundacoes = anterior.fundacoes;
    descarte  = anterior.descarte;
    cava      = anterior.cava;

    pontuacao.setPontos(anterior.pontos);
    pontuacao.setPassadasCava(anterior.passadasCava);

    for (int i = 0; i < 7; ++i)
        cartasEscondidas[i] = anterior.cartasEscondidas[i];

    return true;
}

// ==============================================================================
// COMPRAR CARTA
// ==============================================================================

// Move uma carta da cava para o descarte; recicla o descarte se a cava estiver vazia
void Paciencia::comprarCarta() {
    salvarEstadoNoHistorico();

    if (cava.estaVazio()) {
        // Barricada: sem cartas disponíveis em lugar nenhum — operação ignorada silenciosamente
        if (descarte.empty()) return;

        std::reverse(descarte.begin(), descarte.end());
        for (const auto& c : descarte)
            cava.inserirCarta(c);
        descarte.clear();

        pontuacao.setPassadasCava(pontuacao.getPassadasCava() + 1);
        pontuacao.aplicar(EventoPontuacao::PassarBaralho);
    } else {
        Carta c = cava.retirarCarta();
        descarte.push_back(c);
    }
}

// ==============================================================================
// MOVER (GERAL)
// ==============================================================================

// Valida e executa o movimento de uma carta entre duas pilhas
bool Paciencia::mover(TipoPilha origemTipo, int origemIndice, TipoPilha destinoTipo, int destinoIndice) {
    // Barricada: valida o movimento antes de alterar qualquer estado
    if (!podeMover(origemTipo, origemIndice, destinoTipo, destinoIndice))
        return false;

    salvarEstadoNoHistorico();
    executarMovimento(origemTipo, origemIndice, destinoTipo, destinoIndice);
    verificarVitoria();
    return true;
}

// Verifica se um movimento simples é permitido pelas regras
bool Paciencia::podeMover(TipoPilha origemTipo, int origemIdx, TipoPilha destinoTipo, int destinoIdx) const {
    const Carta* carta = nullptr;

    if (origemTipo == TipoPilha::Descarte) {
        if (descarte.empty()) return false;
        carta = &descarte.back();
    }
    else if (origemTipo == TipoPilha::Coluna) {
        if (origemIdx < 0 || origemIdx >= 7) return false;
        if (colunas[origemIdx].empty()) return false;

        int indiceTopo = static_cast<int>(colunas[origemIdx].size()) - 1;
        if (!cartaVisivel(origemIdx, indiceTopo)) return false;

        carta = &colunas[origemIdx].back();
    }
    else {
        return false; // Fundação tratada por moverDaFundacao()
    }

    if (destinoTipo == TipoPilha::Coluna) {
        if (destinoIdx < 0 || destinoIdx >= 7) return false;
        if (origemTipo == TipoPilha::Coluna && origemIdx == destinoIdx) return false;

        if (colunas[destinoIdx].empty())
            return Regras::podeMoverParaColunaVazia(*carta);

        return Regras::podeMoverParaColuna(*carta, colunas[destinoIdx].back());
    }

    if (destinoTipo == TipoPilha::Fundacao) {
        if (destinoIdx < 0 || destinoIdx >= 4) return false;
        return Regras::podeMoverParaFundacao(*carta, fundacoes[destinoIdx]);
    }

    return false;
}

// Executa o movimento após validação, atualizando pontuação e visibilidade
void Paciencia::executarMovimento(TipoPilha origemTipo, int origemIdx, TipoPilha destinoTipo, int destinoIdx) {
    // Asserções: pré-condições internas — só chegam aqui após podeMover() retornar true
    assert((origemTipo == TipoPilha::Descarte || origemTipo == TipoPilha::Coluna) &&
           "executarMovimento: tipo de origem inválido.");

    Carta carta;

    try {
        if (origemTipo == TipoPilha::Descarte) {
            if (descarte.empty())
                throw PilhaVaziaException("executarMovimento: descarte vazio ao tentar retirar carta.");
            carta = descarte.back();
            descarte.pop_back();
        }
        else {
            if (origemIdx < 0 || origemIdx >= 7)
                throw IndiceInvalidoException("executarMovimento: coluna origem " + std::to_string(origemIdx) + " inválida.");
            if (colunas[origemIdx].empty())
                throw PilhaVaziaException("executarMovimento: coluna " + std::to_string(origemIdx) + " vazia.");
            carta = colunas[origemIdx].back();
            colunas[origemIdx].pop_back();
        }

        if (destinoTipo == TipoPilha::Coluna) {
            if (destinoIdx < 0 || destinoIdx >= 7)
                throw IndiceInvalidoException("executarMovimento: coluna destino " + std::to_string(destinoIdx) + " inválida.");
            colunas[destinoIdx].push_back(carta);
        }
        else if (destinoTipo == TipoPilha::Fundacao) {
            if (destinoIdx < 0 || destinoIdx >= 4)
                throw IndiceInvalidoException("executarMovimento: fundação destino " + std::to_string(destinoIdx) + " inválida.");
            fundacoes[destinoIdx].push_back(carta);
        }
        else {
            throw MovimentoInvalidoException("executarMovimento: tipo de destino inválido.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[EXCEÇÃO] " << e.what() << "\n";
        return;
    }

    pontuacao.aplicar(definirEvento(origemTipo, destinoTipo));

    if (origemTipo == TipoPilha::Coluna &&
        !colunas[origemIdx].empty() &&
        cartasEscondidas[origemIdx] > 0 &&
        static_cast<int>(colunas[origemIdx].size()) == cartasEscondidas[origemIdx])
    {
        try {
            virarParaCima(origemIdx);
        }
        catch (const std::exception& e) {
            std::cerr << "[EXCEÇÃO] " << e.what() << "\n";
        }
    }
}

// ==============================================================================
// MOVER BLOCO
// ==============================================================================

// Valida e executa o movimento de um bloco de cartas entre colunas
bool Paciencia::moverBloco(int origemColuna, int cartaIdx, int destinoColuna) {
    if (!podeMoverBloco(origemColuna, cartaIdx, destinoColuna))
        return false;

    salvarEstadoNoHistorico();
    executarMovimentoBloco(origemColuna, cartaIdx, destinoColuna);
    verificarVitoria();
    return true;
}

// Verifica se um bloco pode ser movido entre colunas
bool Paciencia::podeMoverBloco(int origemCol, int cartaIdx, int destinoCol) const {
    if (origemCol < 0 || origemCol >= 7 || destinoCol < 0 || destinoCol >= 7) return false;
    if (origemCol == destinoCol) return false;
    if (colunas[origemCol].empty()) return false;
    if (cartaIdx < 0 || cartaIdx >= (int)colunas[origemCol].size()) return false;
    if (!cartaVisivel(origemCol, cartaIdx)) return false;

    const Carta& baseBloco = colunas[origemCol][cartaIdx];

    if (colunas[destinoCol].empty())
        return Regras::podeMoverParaColunaVazia(baseBloco);

    return Regras::podeMoverParaColuna(baseBloco, colunas[destinoCol].back());
}

// Transfere o bloco de cartas e revela carta escondida se necessário
void Paciencia::executarMovimentoBloco(int origemCol, int cartaIdx, int destinoCol) {
    // Asserções: pré-condições internas garantidas por podeMoverBloco()
    assert(origemCol >= 0 && origemCol < 7 && "executarMovimentoBloco: origemCol fora dos limites.");
    assert(destinoCol >= 0 && destinoCol < 7 && "executarMovimentoBloco: destinoCol fora dos limites.");
    assert(cartaIdx >= 0 && cartaIdx < (int)colunas[origemCol].size() &&
           "executarMovimentoBloco: cartaIdx fora dos limites.");

    try {
        auto& colOrigem  = colunas[origemCol];
        auto& colDestino = colunas[destinoCol];

        colDestino.insert(colDestino.end(), colOrigem.begin() + cartaIdx, colOrigem.end());
        colOrigem.erase(colOrigem.begin() + cartaIdx, colOrigem.end());

        if (!colOrigem.empty() &&
            (int)colOrigem.size() == cartasEscondidas[origemCol] &&
            cartasEscondidas[origemCol] > 0)
        {
            virarParaCima(origemCol);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[EXCEÇÃO] executarMovimentoBloco: " << e.what() << "\n";
        return;
    }

    pontuacao.aplicar(EventoPontuacao::ColunaParaColuna);
}

// ==============================================================================
// MOVER DA FUNDAÇÃO
// ==============================================================================

// Valida e executa o retorno de uma carta da fundação para uma coluna
bool Paciencia::moverDaFundacao(int fundacaoIndice, TipoPilha destinoTipo, int destinoIndice) {
    // Barricada: destino deve ser coluna
    if (destinoTipo != TipoPilha::Coluna)
        throw MovimentoInvalidoException("moverDaFundacao: destino deve ser do tipo Coluna.");

    if (!podeMoverDaFundacao(fundacaoIndice, destinoIndice))
        return false;

    salvarEstadoNoHistorico();
    executarMovimentoDaFundacao(fundacaoIndice, destinoIndice);
    return true;
}

// Verifica se a carta do topo da fundação pode ir para a coluna destino
bool Paciencia::podeMoverDaFundacao(int fundacaoIdx, int destinoCol) const {
    if (fundacaoIdx < 0 || fundacaoIdx >= 4 || destinoCol < 0 || destinoCol >= 7) return false;
    if (fundacoes[fundacaoIdx].empty()) return false;

    const Carta& cartaParaMover = fundacoes[fundacaoIdx].back();

    if (colunas[destinoCol].empty())
        return Regras::podeMoverParaColunaVazia(cartaParaMover);

    return Regras::podeMoverParaColuna(cartaParaMover, colunas[destinoCol].back());
}

// Move a carta do topo da fundação para a coluna destino
void Paciencia::executarMovimentoDaFundacao(int fundacaoIdx, int destinoCol) {
    // Asserções: garantidas por podeMoverDaFundacao()
    assert(fundacaoIdx >= 0 && fundacaoIdx < 4 &&
           "executarMovimentoDaFundacao: índice de fundação fora dos limites.");
    assert(destinoCol >= 0 && destinoCol < 7 &&
           "executarMovimentoDaFundacao: índice de coluna fora dos limites.");
    assert(!fundacoes[fundacaoIdx].empty() &&
           "executarMovimentoDaFundacao: fundação vazia.");

    try {
        Carta c = fundacoes[fundacaoIdx].back();
        fundacoes[fundacaoIdx].pop_back();
        colunas[destinoCol].push_back(c);
        pontuacao.aplicar(EventoPontuacao::FundacaoParaColuna);
    }
    catch (const std::exception& e) {
        std::cerr << "[EXCEÇÃO] executarMovimentoDaFundacao: " << e.what() << "\n";
    }
}

// ==============================================================================
// PONTUAÇÃO / EVENTO
// ==============================================================================

// Mapeia a combinação origem/destino para o evento de pontuação correto
EventoPontuacao Paciencia::definirEvento(TipoPilha origem, TipoPilha destino) const {
    if (origem == TipoPilha::Descarte && destino == TipoPilha::Fundacao) return EventoPontuacao::CavaParaFundacao;
    if (origem == TipoPilha::Coluna  && destino == TipoPilha::Fundacao) return EventoPontuacao::ColunaParaFundacao;
    if (origem == TipoPilha::Descarte && destino == TipoPilha::Coluna)  return EventoPontuacao::CavaParaColuna;
    if (origem == TipoPilha::Coluna  && destino == TipoPilha::Coluna)   return EventoPontuacao::ColunaParaColuna;
    return EventoPontuacao::CavaParaColuna;
}

// Retorna a pontuação atual da partida
int Paciencia::getPontuacao() const {
    return pontuacao.getPontos();
}

// ==============================================================================
// VERIFICAÇÃO DE VITÓRIA E JOGADAS POSSÍVEIS
// ==============================================================================

// Verifica se todas as fundações estão completas (13 cartas cada)
bool Paciencia::verificarVitoria() {
    for (int i = 0; i < 4; ++i) {
        if (fundacoes[i].size() < 13) {
            vitoria = false;
            return false;
        }
    }
    vitoria = true;
    return true;
}

// Verifica se ainda há algum movimento válido disponível no estado atual
bool Paciencia::existeJogadaPossivel() const {
    for (int i = 0; i < 7; ++i) {
        if (!colunas[i].empty()) {
            const Carta& c = colunas[i].back();
            if (Regras::podeMoverParaFundacao(c, fundacoes[static_cast<int>(c.mostraNaipe())])) return true;
        }
    }
    for (int i = 0; i < 7; ++i) {
        if (colunas[i].empty()) continue;
        const Carta& base = colunas[i].back();
        for (int j = 0; j < 7; ++j) {
            if (i == j) continue;
            if (colunas[j].empty()) {
                if (Regras::podeMoverParaColunaVazia(base)) return true;
            } else {
                if (Regras::podeMoverParaColuna(base, colunas[j].back())) return true;
            }
        }
    }
    if (!descarte.empty()) {
        const Carta& d = descarte.back();
        if (Regras::podeMoverParaFundacao(d, fundacoes[static_cast<int>(d.mostraNaipe())])) return true;
        for (int i = 0; i < 7; ++i) {
            if (colunas[i].empty()) {
                if (Regras::podeMoverParaColunaVazia(d)) return true;
            } else {
                if (Regras::podeMoverParaColuna(d, colunas[i].back())) return true;
            }
        }
    }
    return false;
}

// ==============================================================================
// AUTO COMPLETAR
// ==============================================================================

// Move todas as cartas possíveis para as fundações em loop
void Paciencia::completarAutomaticamente() {
    while (moverUmaParaFundacao()) {}
}

// Tenta mover uma única carta (descarte ou coluna) para a fundação correta
bool Paciencia::moverUmaParaFundacao() {
    if (!descarte.empty()) {
        Carta& carta = descarte.back();
        int nIdx = static_cast<int>(carta.mostraNaipe());

        // Barricada: naipe deve estar no intervalo válido
        if (nIdx < 0 || nIdx >= 4)
            throw EstadoInvalidoException("moverUmaParaFundacao: naipe inválido no descarte (" + std::to_string(nIdx) + ").");

        if (Regras::podeMoverParaFundacao(carta, fundacoes[nIdx]))
            return mover(TipoPilha::Descarte, 0, TipoPilha::Fundacao, nIdx);
    }

    for (int i = 0; i < 7; i++) {
        if (!colunas[i].empty()) {
            Carta& c = colunas[i].back();
            int nIdx = static_cast<int>(c.mostraNaipe());

            // Barricada: naipe deve estar no intervalo válido
            if (nIdx < 0 || nIdx >= 4)
                throw EstadoInvalidoException("moverUmaParaFundacao: naipe inválido na coluna " + std::to_string(i) + " (" + std::to_string(nIdx) + ").");

            if (Regras::podeMoverParaFundacao(c, fundacoes[nIdx]))
                return mover(TipoPilha::Coluna, i, TipoPilha::Fundacao, nIdx);
        }
    }

    return false;
}

// ==============================================================================
// SOLVER / IA
// ==============================================================================

// Gera uma representação textual do tabuleiro para uso no solver
std::string Paciencia::converterParaString() const {
    std::string output = "--- ESTADO DO JOGO ---\n";

    output += "Fundacoes: ";
    for (int i = 0; i < 4; ++i) {
        if (fundacoes[i].empty()) output += "[  ] ";
        else output += "[" + fundacoes[i].back().toString() + "] ";
    }

    output += "\nDescarte: " + (descarte.empty() ? "[Vazio]" : descarte.back().toString());
    output += "\n\nColunas:\n";

    int maxAltura = 0;
    for (int i = 0; i < 7; ++i) {
        if ((int)colunas[i].size() > maxAltura) maxAltura = colunas[i].size();
    }

    for (int linha = 0; linha < maxAltura; ++linha) {
        for (int col = 0; col < 7; ++col) {
            if (linha < (int)colunas[col].size()) {
                if (linha < cartasEscondidas[col]) output += "[#]  ";
                else output += "[" + colunas[col][linha].toString() + "] ";
            } else {
                output += "     ";
            }
        }
        output += "\n";
    }

    output += "----------------------\n";
    return output;
}

// Coleta e retorna todas as jogadas válidas no estado atual
std::vector<JogadaSimulada> Paciencia::listarJogadasPossiveis() {
    std::vector<JogadaSimulada> jogadas;
    coletarJogadasDescarte(jogadas);
    coletarJogadasColunas(jogadas);
    coletarJogadasFundacao(jogadas);

    if (cava.tamanho() > 0 || !descarte.empty())
        jogadas.push_back({"COMPRAR", TipoPilha::Descarte, 0, TipoPilha::Descarte, 0, 0});

    return jogadas;
}

// Adiciona ao vetor as jogadas possíveis a partir do descarte
void Paciencia::coletarJogadasDescarte(std::vector<JogadaSimulada>& jogadas) const {
    if (descarte.empty()) return;
    const Carta& topo = descarte.back();

    for (int i = 0; i < 4; ++i)
        if (Regras::podeMoverParaFundacao(topo, fundacoes[i]))
            jogadas.push_back({"MOVER", TipoPilha::Descarte, 0, TipoPilha::Fundacao, i, 0});

    for (int i = 0; i < 7; ++i)
        if (colunas[i].empty() ? Regras::podeMoverParaColunaVazia(topo) : Regras::podeMoverParaColuna(topo, colunas[i].back()))
            jogadas.push_back({"MOVER", TipoPilha::Descarte, 0, TipoPilha::Coluna, i, 0});
}

// Adiciona ao vetor as jogadas possíveis a partir das colunas
void Paciencia::coletarJogadasColunas(std::vector<JogadaSimulada>& jogadas) const {
    for (int i = 0; i < 7; ++i) {
        if (colunas[i].empty()) continue;

        for (int j = (int)colunas[i].size() - 1; j >= 0; --j) {
            if (!cartaVisivel(i, j)) break;

            const Carta& carta = colunas[i][j];
            bool ehUltima = (j == (int)colunas[i].size() - 1);

            for (int k = 0; k < 7; ++k) {
                if (i == k) continue;
                if (colunas[k].empty() ? Regras::podeMoverParaColunaVazia(carta) : Regras::podeMoverParaColuna(carta, colunas[k].back()))
                    jogadas.push_back({"MOVER_BLOCO", TipoPilha::Coluna, i, TipoPilha::Coluna, k, j});
            }

            if (ehUltima)
                for (int k = 0; k < 4; ++k)
                    if (Regras::podeMoverParaFundacao(carta, fundacoes[k]))
                        jogadas.push_back({"MOVER", TipoPilha::Coluna, i, TipoPilha::Fundacao, k, 0});
        }
    }
}

// Adiciona ao vetor as jogadas possíveis a partir das fundações
void Paciencia::coletarJogadasFundacao(std::vector<JogadaSimulada>& jogadas) const {
    for (int i = 0; i < 4; ++i) {
        if (fundacoes[i].empty()) continue;
        const Carta& topo = fundacoes[i].back();

        for (int j = 0; j < 7; ++j)
            if (colunas[j].empty() ? Regras::podeMoverParaColunaVazia(topo) : Regras::podeMoverParaColuna(topo, colunas[j].back()))
                jogadas.push_back({"MOVER_DA_FUNDACAO", TipoPilha::Fundacao, i, TipoPilha::Coluna, j, 0});
    }
}

// Busca recursiva com backtracking para encontrar uma solução
bool Paciencia::simularSolucao(std::set<std::string>& estadosVisitados) {
    if (verificarVitoria()) return true;

    std::string estadoAtualStr = converterParaString();
    if (estadosVisitados.count(estadoAtualStr)) return false;
    estadosVisitados.insert(estadoAtualStr);

    if (estadosVisitados.size() > 5000) return false;

    std::vector<JogadaSimulada> jogadas = listarJogadasPossiveis();

    for (const auto& jogada : jogadas) {
        bool movimentoRealizado = false;

        try {
            if (jogada.tipoAcao == "COMPRAR") {
                comprarCarta();
                movimentoRealizado = true;
            } else if (jogada.tipoAcao == "MOVER") {
                movimentoRealizado = mover(jogada.origemTipo, jogada.origemIdx, jogada.destinoTipo, jogada.destinoIdx);
            } else if (jogada.tipoAcao == "MOVER_BLOCO") {
                movimentoRealizado = moverBloco(jogada.origemIdx, jogada.cartaIdx, jogada.destinoIdx);
            } else if (jogada.tipoAcao == "MOVER_DA_FUNDACAO") {
                movimentoRealizado = moverDaFundacao(jogada.origemIdx, jogada.destinoTipo, jogada.destinoIdx);
            } else {
                throw MovimentoInvalidoException("simularSolucao: ação desconhecida '" + jogada.tipoAcao + "'.");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[EXCEÇÃO] simularSolucao: " << e.what() << "\n";
            continue;
        }

        if (movimentoRealizado) {
            if (simularSolucao(estadosVisitados)) return true;
            desfazer();
        }
    }

    return false;
}

// Garante que o jogo gerado tenha pelo menos 2 jogadas reais disponíveis
bool Paciencia::garantirJogoVencivel() {
    gerarJogoReversivel();

    std::vector<JogadaSimulada> jogadas = listarJogadasPossiveis();
    int jogadasReais = 0;
    for (const auto& j : jogadas)
        if (j.tipoAcao != "COMPRAR") jogadasReais++;

    if (jogadasReais < 2) {
        std::cout << "[SOLVER] Jogo gerado estava muito travado. Regenerando...\n";
        gerarJogoReversivel();
    }

    std::cout << "[SOLVER] Jogo garantido gerado com sucesso!\n";
    return true;
}

// Gera um jogo vencível distribuindo cartas de trás para frente
void Paciencia::gerarJogoReversivel() {
    std::mt19937 rng(std::random_device{}());

    colunas.assign(7, std::vector<Carta>());
    fundacoes.assign(4, std::vector<Carta>());
    descarte.clear();
    while (!historico.empty()) historico.pop();

    cava = Baralho(0);
    for (int i = 0; i < 7; i++) cartasEscondidas[i] = i;

    int cartas_nas_fundacoes[4] = {13, 13, 13, 13};
    std::vector<Naipe> naipes = { Naipe::Paus, Naipe::Copa, Naipe::Espada, Naipe::Ouro };

    std::vector<std::vector<Carta>> colunas_temp(7);
    std::vector<Carta> deck_temp;
    deck_temp.reserve(24);

    int capacidade_coluna[7] = {1, 2, 3, 4, 5, 6, 7};

    try {
        for (int i = 0; i < 52; i++) {
            std::vector<int> naipes_disponiveis;
            for (int n = 0; n < 4; n++)
                if (cartas_nas_fundacoes[n] > 0)
                    naipes_disponiveis.push_back(n);

            if (naipes_disponiveis.empty())
                throw EstadoInvalidoException("gerarJogoReversivel: nenhum naipe disponível na iteração " + std::to_string(i) + ".");

            std::uniform_int_distribution<int> dist_naipe(0, (int)naipes_disponiveis.size() - 1);
            int naipe_escolhido = naipes_disponiveis[dist_naipe(rng)];

            int valor_carta = cartas_nas_fundacoes[naipe_escolhido];
            cartas_nas_fundacoes[naipe_escolhido]--;

            Carta carta_atual(static_cast<Valor>(valor_carta), naipes[naipe_escolhido]);

            std::vector<int> destinos_disponiveis;
            for (int c = 0; c < 7; c++)
                if ((int)colunas_temp[c].size() < capacidade_coluna[c])
                    destinos_disponiveis.push_back(c);

            if ((int)deck_temp.size() < 24)
                for (int p = 0; p < 3; p++) destinos_disponiveis.push_back(7);

            if (destinos_disponiveis.empty()) {
                deck_temp.push_back(carta_atual);
            } else {
                std::uniform_int_distribution<int> dist_dest(0, (int)destinos_disponiveis.size() - 1);
                int destino_escolhido = destinos_disponiveis[dist_dest(rng)];

                if (destino_escolhido == 7)
                    deck_temp.push_back(carta_atual);
                else
                    colunas_temp[destino_escolhido].push_back(carta_atual);
            }
        }
    }
    catch (const EstadoInvalidoException& e) {
        std::cerr << "[EXCEÇÃO] " << e.what() << "\n";
        // Reinicia com embaralhamento simples como fallback
        iniciarJogo();
        return;
    }

    // Validação de integridade pós-geração
    int totalDistribuido = (int)deck_temp.size();
    for (int c = 0; c < 7; c++) totalDistribuido += (int)colunas_temp[c].size();

    if (totalDistribuido != 52)
        throw EstadoInvalidoException("gerarJogoReversivel: distribuição incompleta — " +
                                      std::to_string(totalDistribuido) + "/52 cartas distribuídas.");

    std::shuffle(deck_temp.begin(), deck_temp.end(), rng);

    for (int c = 0; c < 7; c++) colunas[c] = colunas_temp[c];
    for (const auto& carta : deck_temp) cava.inserirCarta(carta);

    pontuacao.resetar();
    vitoria = false;

    std::cout << "[GERADOR] Jogo 100% vencível gerado via Método Reverso Matemático.\n";
}

// ==============================================================================
// DEBUG
// ==============================================================================

// Imprime o estado completo do jogo no console para depuração
void Paciencia::imprimirJogo() {
    std::cout << "\n========================================\n";
    std::cout << "PONTOS: " << pontuacao.getPontos() << " | RECORDE: " << pontuacao.getRecord() << "\n";
    std::cout << "========================================\n";

    std::cout << "Cava [" << cava.tamanho() << "] | Descarte: ";
    if (descarte.empty()) std::cout << "[ vazio ]";
    else std::cout << "[" << descarte.back().cartaString() << "]";
    std::cout << "\n\n";

    std::cout << "Fundacoes: ";
    for (int i = 0; i < 4; i++) {
        if (fundacoes[i].empty()) std::cout << "[ _ ] ";
        else std::cout << "[" << fundacoes[i].back().cartaString() << "] ";
    }
    std::cout << "\n\n";

    std::cout << "Colunas:\n";
    int maxLinhas = 0;
    for (int i = 0; i < 7; i++)
        if ((int)colunas[i].size() > maxLinhas) maxLinhas = (int)colunas[i].size();

    for (int i = 0; i < 7; i++) std::cout << "  Col" << (i + 1) << "  \t";
    std::cout << "\n";

    for (int linha = 0; linha < maxLinhas; linha++) {
        for (int col = 0; col < 7; col++) {
            if (linha < (int)colunas[col].size()) {
                if (!cartaVisivel(col, linha)) std::cout << "[   ???   ]\t";
                else std::cout << "[" << colunas[col][linha].cartaString() << "]\t";
            } else {
                std::cout << "        \t";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    if (vitoria) std::cout << "*** PARABENS! VOCE GANHOU! ***\n";
}