#pragma once
#include "Mesa.hpp"
#include "Carta.hpp"
#include "Placar.hpp"

class Carta; ///< Declaração antecipada para evitar inclusão circular

/**
 * @brief Gerencia a lógica completa de uma partida do jogo FDP (Fodinha de Paus).
 *
 * Especialização de Mesa que controla o fluxo do jogo: rodadas, apostas,
 * jogadas de cartas, apuração de vazas e gerenciamento do placar.
 * Comunica-se com o servidor WebSocket para transmitir o estado da partida.
 */
class MesaFDP : public Mesa {
private:
    std::vector<Carta*> cartasNaMesa_; ///< Cartas jogadas na vaza atual
    Carta cartaVira_;                  ///< Carta vira que define os trunfos da rodada
    int indicePrimeiro_;               ///< Índice do primeiro jogador da rodada
    int cartasNaRodada_;               ///< Quantidade de cartas por jogador na rodada atual
    Placar* placar_partida_;           ///< Ponteiro para o placar da partida
    bool cartasSubindo_;               ///< Indica se a quantidade de cartas está aumentando
    int jogadorDaVezIndex_ = 0;        ///< Índice do jogador com a vez atual
    int totalApostasRodada_ = 0;       ///< Soma total das apostas feitas na rodada
    int jogadoresQueJaApostaram_ = 0;  ///< Quantidade de jogadores que já apostaram
    std::vector<int> ordemJogadoresDaVaza_; ///< Ordem dos jogadores na vaza atual

public:
    /**
     * @brief Construtor.
     * @param baralho Ponteiro para o baralho do jogo.
     * @param placar  Ponteiro para o placar da partida.
     */
    MesaFDP(BaralhoSujo* baralho, Placar* placar);

    /** @brief Destrutor virtual. */
    virtual ~MesaFDP();

    // =========================================================
    /// @name Controle da Partida
    // =========================================================
    ///@{

    /** @brief Inicia uma nova partida, configurando o estado inicial. */
    void iniciarPartida();

    /**
     * @brief Prepara uma nova partida com o número especificado de jogadores.
     * @param qtdJogadores Quantidade de jogadores na partida.
     */
    void prepararNovaPartida(int qtdJogadores);

    /** @brief Inicia uma nova rodada, distribuindo cartas e definindo a vira. */
    void iniciarRodada();

    /** @brief Finaliza a rodada atual e atualiza o placar. */
    void finalizarRodada();

    /** @brief Atualiza a quantidade de cartas por rodada (crescente ou decrescente). */
    void atualizarQtdCartas();

    /** @brief Anuncia o vencedor da partida. */
    void anunciarVencedor();

    /** @brief Recolhe as cartas da mesa ao fim de uma vaza. */
    void recolherCartas();

    ///@}

    // =========================================================
    /// @name Fase de Apostas
    // =========================================================
    ///@{

    /** @brief Inicia a fase de apostas da rodada atual. */
    void iniciarFaseApostas();

    /**
     * @brief Registra a aposta de um jogador.
     * @param apostaDesejada Valor apostado pelo jogador.
     * @return true se a aposta foi registrada com sucesso.
     */
    bool registrarAposta(int apostaDesejada);

    /**
     * @brief Verifica se todos os jogadores já realizaram suas apostas.
     * @return true se a fase de apostas está encerrada.
     */
    bool faseApostasFinalizada();

    /**
     * @brief Retorna a aposta proibida para o último jogador a apostar.
     *
     * Impede que a soma das apostas seja igual ao número de cartas da rodada.
     * @return Valor proibido para a aposta.
     */
    int getApostaProibida() const;

    ///@}

    // =========================================================
    /// @name Fase de Cartas (Vazas)
    // =========================================================
    ///@{

    /** @brief Inicia a fase de jogada de cartas após as apostas. */
    void iniciarFaseDeCartas();

    /**
     * @brief Registra a jogada de uma carta pelo jogador da vez.
     * @param indiceCartaNaMao Índice da carta na mão do jogador.
     * @return true se a jogada foi registrada com sucesso.
     */
    bool jogarCarta(int indiceCartaNaMao);

    /**
     * @brief Verifica se todos os jogadores já jogaram na vaza atual.
     * @return true se a vaza está completa.
     */
    bool vazaFinalizada();

    /** @brief Apura e registra o vencedor da vaza atual. */
    void apurarVencedorDaVaza();

    /** @brief Apura os resultados da rodada e aplica dano aos perdedores. */
    void apurarResultados();

    /**
     * @brief Verifica se a rodada atual foi finalizada.
     * @return true se não há mais vazas a jogar na rodada.
     */
    bool rodadaFinalizada() const;

    ///@}

    // =========================================================
    /// @name Getters
    // =========================================================
    ///@{

    /** @return Ponteiro para o jogador com a vez atual. */
    JogadorFDP* getJogadorDaVez();

    /** @return Índice do jogador com a vez atual. */
    int getJogadorDaVezIndex() const;

    /** @return Quantidade de cartas por jogador na rodada atual. */
    int getCartasNaRodada() const;

    /** @return Soma total das apostas feitas na rodada. */
    int getTotalApostasRodada() const { return totalApostasRodada_; }

    /** @return Quantidade de jogadores que já apostaram na rodada. */
    int getJogadoresQueJaApostaram() const { return jogadoresQueJaApostaram_; }

    /** @return Carta vira da rodada atual (por cópia). */
    Carta getCartaVira() const { return cartaVira_; }

    /** @return Referência constante para as cartas jogadas na vaza atual. */
    const std::vector<Carta*>& getCartasNaMesa() const { return cartasNaMesa_; }

    /**
     * @brief Retorna um resumo textual do resultado da rodada.
     * @return Vetor de strings descrevendo o resultado de cada jogador.
     */
    std::vector<std::string> obterResumoRodada();

    ///@}
};