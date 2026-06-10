#ifndef PACIENCIA_H
#define PACIENCIA_H

#include <vector>
#include <stack>
#include <string>
#include <map>
#include <set>
#include "Baralho.hpp"
#include "Pontuacao.hpp"

/**
 * @brief Identifica o tipo de pilha no tabuleiro do jogo.
 *
 * Usado para indicar a origem e o destino de movimentos de cartas.
 */
enum class TipoPilha {
    Coluna,   ///< Uma das sete colunas do tableau
    Descarte, ///< Pilha de descarte da cava
    Fundacao  ///< Uma das quatro pilhas de fundação
};

/**
 * @brief Representa a assinatura completa de uma jogada simulada.
 *
 * Utilizada pelo solver/IA para descrever e executar movimentos
 * durante a busca por soluções.
 */
struct JogadaSimulada {
    std::string tipoAcao;  ///< Tipo da ação: "MOVER", "MOVER_BLOCO", "MOVER_DA_FUNDACAO", "COMPRAR"
    TipoPilha origemTipo;  ///< Tipo da pilha de origem
    int origemIdx;         ///< Índice da pilha de origem
    TipoPilha destinoTipo; ///< Tipo da pilha de destino
    int destinoIdx;        ///< Índice da pilha de destino
    int cartaIdx;          ///< Índice da carta dentro da pilha (usado em MOVER_BLOCO)
};

/**
 * @brief Classe principal do jogo Paciência (Klondike Solitaire).
 *
 * Gerencia todo o estado do jogo: tabuleiro, colunas, fundações, cava,
 * descarte, pontuação, histórico de jogadas e lógica do solver.
 * Expõe uma interface pública usada pelo servidor WebSocket para
 * processar ações do jogador e transmitir o estado atualizado.
 */
class Paciencia {
private:

    /**
     * @brief Snapshot completo do estado do jogo em um dado momento.
     *
     * Armazenado no histórico para permitir a operação de desfazer.
     */
    struct EstadoJogo {
        std::vector<std::vector<Carta>> colunas;    ///< Estado das 7 colunas
        std::vector<std::vector<Carta>> fundacoes;  ///< Estado das 4 fundações
        std::vector<Carta> descarte;                ///< Estado do descarte
        Baralho cava;                               ///< Estado da cava
        int pontos;                                 ///< Pontuação no momento do snapshot
        int passadasCava;                           ///< Número de passadas pela cava
        int cartasEscondidas[7];                    ///< Quantidade de cartas viradas para baixo em cada coluna
    };

    bool vitoria;                                   ///< Indica se o jogo foi vencido
    std::vector<std::vector<Carta>> colunas;        ///< As 7 colunas do tableau
    std::vector<std::vector<Carta>> fundacoes;      ///< As 4 pilhas de fundação (uma por naipe)
    std::vector<Carta> descarte;                    ///< Pilha de descarte
    Baralho cava;                                   ///< Baralho da cava (estoque)
    int cartasEscondidas[7];                        ///< Cartas viradas para baixo em cada coluna

    std::stack<EstadoJogo> historico;               ///< Pilha de estados para desfazer jogadas
    Pontuacao pontuacao;                            ///< Sistema de pontuação e recorde

    /** @brief Salva o estado atual no histórico antes de uma jogada. */
    void salvarEstadoNoHistorico();

    /** @brief Restaura o estado mais recente do histórico (usado por desfazer). */
    void restaurarEstadoDoHistorico();

    /** @brief Coleta jogadas possíveis a partir do descarte. @param jogadas Vetor de saída. */
    void coletarJogadasDescarte(std::vector<JogadaSimulada>& jogadas) const;

    /** @brief Coleta jogadas possíveis a partir das colunas. @param jogadas Vetor de saída. */
    void coletarJogadasColunas(std::vector<JogadaSimulada>& jogadas) const;

    /** @brief Coleta jogadas possíveis a partir das fundações. @param jogadas Vetor de saída. */
    void coletarJogadasFundacao(std::vector<JogadaSimulada>& jogadas) const;

    /**
     * @brief Verifica se um movimento simples (carta única) é válido.
     * @param origemTipo Tipo da pilha de origem.
     * @param origemIdx  Índice da pilha de origem.
     * @param destinoTipo Tipo da pilha de destino.
     * @param destinoIdx  Índice da pilha de destino.
     * @return true se o movimento é permitido pelas regras.
     */
    bool podeMover(TipoPilha origemTipo, int origemIdx, TipoPilha destinoTipo, int destinoIdx) const;

    /**
     * @brief Verifica se um bloco de cartas pode ser movido entre colunas.
     * @param origemCol Coluna de origem.
     * @param cartaIdx  Índice da carta base do bloco.
     * @param destinoCol Coluna de destino.
     * @return true se o movimento é permitido.
     */
    bool podeMoverBloco(int origemCol, int cartaIdx, int destinoCol) const;

    /**
     * @brief Verifica se a carta do topo de uma fundação pode voltar para uma coluna.
     * @param fundacaoIdx Índice da fundação de origem.
     * @param destinoCol  Índice da coluna de destino.
     * @return true se o movimento é permitido.
     */
    bool podeMoverDaFundacao(int fundacaoIdx, int destinoCol) const;

    /**
     * @brief Executa um movimento simples entre pilhas, sem validação prévia.
     * @param origemTipo  Tipo da pilha de origem.
     * @param origemIdx   Índice da pilha de origem.
     * @param destinoTipo Tipo da pilha de destino.
     * @param destinoIdx  Índice da pilha de destino.
     */
    void executarMovimento(TipoPilha origemTipo, int origemIdx, TipoPilha destinoTipo, int destinoIdx);

    /**
     * @brief Executa o movimento de um bloco de cartas entre colunas.
     * @param origemCol  Coluna de origem.
     * @param cartaIdx   Índice da carta base do bloco.
     * @param destinoCol Coluna de destino.
     */
    void executarMovimentoBloco(int origemCol, int cartaIdx, int destinoCol);

    /**
     * @brief Executa o retorno de uma carta da fundação para uma coluna.
     * @param fundacaoIdx Índice da fundação de origem.
     * @param destinoCol  Índice da coluna de destino.
     */
    void executarMovimentoDaFundacao(int fundacaoIdx, int destinoCol);

    /**
     * @brief Determina o evento de pontuação correspondente a um movimento.
     * @param origem Tipo da pilha de origem.
     * @param destino Tipo da pilha de destino.
     * @return O EventoPontuacao correspondente.
     */
    EventoPontuacao definirEvento(TipoPilha origem, TipoPilha destino) const;

public:

    /** @brief Construtor. Inicializa e inicia um novo jogo automaticamente. */
    Paciencia();

    /** @brief Destrutor. Recursos liberados automaticamente pelos contêineres STL. */
    ~Paciencia();

    // =========================================================
    /// @name Ciclo de Vida do Jogo
    // =========================================================
    ///@{

    /** @brief Reinicia o jogo completamente, embaralhando um novo baralho. */
    void iniciarJogo();

    /**
     * @brief Gera um novo jogo garantidamente vencível via método reverso.
     *
     * Distribui as cartas de trás para frente a partir de um estado de
     * vitória, garantindo que sempre existe pelo menos uma solução.
     */
    void gerarJogoReversivel();

    /**
     * @brief Compra uma carta da cava para o descarte.
     *
     * Se a cava estiver vazia, recicla o descarte de volta para a cava.
     */
    void comprarCarta();

    /**
     * @brief Desfaz a última jogada realizada.
     * @return true se havia estado no histórico para restaurar.
     */
    bool desfazer();

    ///@}

    // =========================================================
    /// @name Comandos de Movimento
    // =========================================================
    ///@{

    /**
     * @brief Move uma carta simples entre duas pilhas.
     * @param origemTipo    Tipo da pilha de origem.
     * @param origemIndice  Índice da pilha de origem.
     * @param destinoTipo   Tipo da pilha de destino.
     * @param destinoIndice Índice da pilha de destino.
     * @return true se o movimento foi realizado com sucesso.
     */
    bool mover(TipoPilha origemTipo, int origemIndice, TipoPilha destinoTipo, int destinoIndice);

    /**
     * @brief Move um bloco de cartas de uma coluna para outra.
     * @param origemColuna Coluna de origem.
     * @param cartaIdx     Índice da carta base do bloco na coluna.
     * @param destinoColuna Coluna de destino.
     * @return true se o movimento foi realizado com sucesso.
     */
    bool moverBloco(int origemColuna, int cartaIdx, int destinoColuna);

    /**
     * @brief Move a carta do topo de uma fundação de volta para uma coluna.
     * @param fundacaoIndice Índice da fundação de origem.
     * @param destinoTipo    Tipo da pilha de destino (deve ser Coluna).
     * @param destinoIndice  Índice da coluna de destino.
     * @return true se o movimento foi realizado com sucesso.
     */
    bool moverDaFundacao(int fundacaoIndice, TipoPilha destinoTipo, int destinoIndice);

    /**
     * @brief Tenta mover uma carta para a fundação correta automaticamente.
     *
     * Verifica o descarte e depois as colunas, movendo a primeira carta
     * elegível encontrada.
     * @return true se algum movimento foi realizado.
     */
    bool moverUmaParaFundacao();

    /**
     * @brief Vira a carta do topo de uma coluna para cima (revela-a).
     * @param coluna Índice da coluna (0–6).
     */
    void virarParaCima(int coluna);

    ///@}

    // =========================================================
    /// @name Verificações e IA
    // =========================================================
    ///@{

    /**
     * @brief Verifica se ainda existe alguma jogada possível no estado atual.
     * @return true se há pelo menos um movimento válido disponível.
     */
    bool existeJogadaPossivel() const;

    /**
     * @brief Verifica se todas as fundações estão completas (vitória).
     * @return true se o jogo foi vencido.
     */
    bool verificarVitoria();

    /**
     * @brief Verifica se uma carta específica está visível (virada para cima).
     * @param colunaIdx Índice da coluna.
     * @param cartaIdx  Índice da carta na coluna.
     * @return true se a carta está visível.
     */
    bool cartaVisivel(int colunaIdx, int cartaIdx) const;

    /**
     * @brief Verifica se uma carta está exposta (é a última da coluna visível).
     * @param coluna Índice da coluna.
     * @param linha  Índice da linha.
     * @return true se a carta está exposta.
     */
    bool estaExposta(int coluna, int linha) const;

    /**
     * @brief Completa automaticamente o jogo movendo todas as cartas para as fundações.
     *
     * Chama moverUmaParaFundacao() em loop até não restar movimentos.
     */
    void completarAutomaticamente();

    /**
     * @brief Lista todas as jogadas possíveis no estado atual.
     * @return Vetor de JogadaSimulada com todos os movimentos válidos.
     */
    std::vector<JogadaSimulada> listarJogadasPossiveis();

    /**
     * @brief Tenta encontrar uma solução para o estado atual via backtracking.
     * @param estadosVisitados Conjunto de estados já explorados (evita loops).
     * @return true se uma solução foi encontrada.
     */
    bool simularSolucao(std::set<std::string>& estadosVisitados);

    /**
     * @brief Garante que o jogo gerado seja vencível, regenerando se necessário.
     * @return true sempre (garante pelo menos um jogo válido).
     */
    bool garantirJogoVencivel();

    ///@}

    // =========================================================
    /// @name Getters — Interface para o Servidor WebSocket
    // =========================================================
    ///@{

    /** @return Referência constante para as 7 colunas do tableau. */
    const std::vector<std::vector<Carta>>& getColunas() const { return colunas; }

    /** @return Referência constante para as 4 pilhas de fundação. */
    const std::vector<std::vector<Carta>>& getFundacoes() const { return fundacoes; }

    /** @return Referência constante para o descarte. */
    const std::vector<Carta>& getDescarte() const { return descarte; }

    /** @return Número de cartas restantes na cava. */
    int getCavaTamanho() const { return cava.tamanho(); }

    /**
     * @brief Retorna o número de cartas escondidas em uma coluna.
     * @param i Índice da coluna (0–6).
     * @return Quantidade de cartas viradas para baixo, ou 0 se índice inválido.
     */
    int getCartasEscondidas(int i) const { return (i >= 0 && i < 7) ? cartasEscondidas[i] : 0; }

    /** @return Pontuação atual do jogador. */
    int getPontuacao() const;

    /** @return true se o jogo foi vencido. */
    bool getVitoria() const { return vitoria; }

    ///@}

    // =========================================================
    /// @name Recorde
    // =========================================================
    ///@{

    /** @return O recorde de pontuação salvo em disco. */
    int getRecord() const { return pontuacao.getRecord(); }

    /**
     * @brief Salva a pontuação atual como recorde se for maior que o anterior.
     * @return true se o recorde foi atualizado.
     */
    bool salvarRecord() { return pontuacao.salvarRecord(); }

    ///@}

    // =========================================================
    /// @name Debug
    // =========================================================
    ///@{

    /** @brief Imprime o estado atual do jogo no console (para depuração). */
    void imprimirJogo();

    /**
     * @brief Converte o estado do jogo para uma string legível.
     * @return String representando o tabuleiro atual.
     */
    std::string converterParaString() const;

    ///@}
};

#endif