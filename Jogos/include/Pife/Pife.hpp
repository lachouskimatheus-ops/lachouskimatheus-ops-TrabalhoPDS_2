#ifndef PIFE_HPP
#define PIFE_HPP

#include "Carta.hpp"
#include "Baralho.hpp"
#include "JogadorPife.hpp"
#include "RegrasPife.hpp"
#include <vector>

/**
 * @brief Representa a fase atual do turno de um jogador no Pife.
 */
enum class FaseTurno {
    AguardandoCompra,   ///< Jogador ainda não comprou carta neste turno
    AguardandoDescarte, ///< Jogador comprou e deve descartar uma carta
    Finalizado          ///< Turno encerrado (jogador bateu ou passou a vez)
};

/**
 * @brief Classe principal que gerencia uma partida de Pife.
 *
 * Controla o fluxo completo do jogo: distribuição de cartas, turnos,
 * compras do baralho e da mesa, descartes, verificação de vitória (bater)
 * e alternância entre jogadores. Expõe uma interface para o servidor
 * WebSocket processar as ações dos jogadores.
 */
class Pife {
private:
    Baralho baralho_;                  ///< Baralho de compra
    std::vector<JogadorPife> jogadores_; ///< Jogadores participantes da partida
    std::vector<Carta> mesa_;          ///< Pilha de descarte (mesa)
    Carta vira_;                       ///< Carta vira que define o coringa da rodada
    int jogadorAtual_;                 ///< Índice do jogador com a vez atual
    int vencedor_;                     ///< Índice do jogador vencedor (-1 se não houve)
    bool jogoFinalizado_;              ///< Indica se a partida foi encerrada
    FaseTurno faseTurno_;              ///< Fase atual do turno em andamento

    /**
     * @brief Distribui as cartas iniciais para todos os jogadores.
     */
    void distribuirCartas();

    /**
     * @brief Verifica se um ID de jogador é válido.
     * @param idJogador ID a verificar.
     * @return true se o ID está dentro do intervalo de jogadores.
     */
    bool jogadorValido(int idJogador) const;

    /**
     * @brief Reabastece o baralho com as cartas da mesa quando ele esvazia.
     *
     * Mantém a carta do topo da mesa como novo topo e embaralha o restante.
     */
    void reporBaralhoComDescarte();

public:
    /**
     * @brief Construtor. Inicializa e inicia uma partida com o número de jogadores dado.
     * @param quantidadeJogadores Número de jogadores (tipicamente 2–4).
     */
    Pife(int quantidadeJogadores);

    // =========================================================
    /// @name Ações do Jogo
    // =========================================================
    ///@{

    /**
     * @brief Jogador compra uma carta do baralho.
     * @param idJogador ID do jogador que está comprando.
     * @return true se a ação foi realizada com sucesso.
     */
    bool comprarBaralho(int idJogador);

    /**
     * @brief Jogador compra a carta do topo da mesa (descarte).
     * @param idJogador ID do jogador que está comprando.
     * @return true se a ação foi realizada com sucesso.
     */
    bool comprarMesa(int idJogador);

    /**
     * @brief Jogador descarta uma carta da mão para a mesa.
     * @param idJogador   ID do jogador que está descartando.
     * @param indiceCarta Índice da carta na mão do jogador.
     * @return true se o descarte foi realizado com sucesso.
     */
    bool colocarNaMesa(int idJogador, int indiceCarta);

    /**
     * @brief Organiza a mão do jogador por naipe e valor.
     * @param idJogador ID do jogador.
     * @return true se a operação foi realizada com sucesso.
     */
    bool organizarMao(int idJogador);

    /**
     * @brief Jogador declara vitória (bate).
     *
     * Verifica se a mão do jogador forma combinações válidas e,
     * em caso positivo, encerra a partida.
     * @param idJogador ID do jogador que está batendo.
     * @return true se a mão é válida e o jogo foi encerrado.
     */
    bool bati(int idJogador);

    /** @brief Passa a vez para o próximo jogador. */
    void proximoJogador();

    ///@}

    // =========================================================
    /// @name Verificação de Ações Permitidas
    // =========================================================
    ///@{

    /** @return true se o jogador pode comprar do baralho no momento. */
    bool podeComprarBaralho(int idJogador) const;

    /** @return true se o jogador pode comprar da mesa no momento. */
    bool podeComprarMesa(int idJogador) const;

    /** @return true se o jogador pode descartar uma carta no momento. */
    bool podeColocarNaMesa(int idJogador) const;

    /** @return true se o jogador pode declarar vitória (bater) no momento. */
    bool podeBater(int idJogador) const;

    ///@}

    // =========================================================
    /// @name Consulta do Estado da Partida
    // =========================================================
    ///@{

    /** @return true se a partida foi encerrada. */
    bool jogoFinalizado() const;

    /** @return Índice do jogador com a vez atual. */
    int consultarIndiceJogadorAtual() const;

    /** @return Índice do jogador vencedor, ou -1 se o jogo ainda não terminou. */
    int consultarVencedor() const;

    /** @return Fase atual do turno em andamento. */
    FaseTurno consultarFaseTurno() const;

    /** @return Número de jogadores na partida. */
    int numeroDeJogadores() const;

    /** @return Quantidade de cartas restantes no baralho. */
    int quantidadeCartasBaralho() const;

    /**
     * @brief Verifica se uma carta é coringa na rodada atual.
     * @param carta Carta a verificar.
     * @return true se a carta é coringa com base na vira.
     */
    bool cartaEhCoringa(const Carta& carta) const;

    ///@}

    // =========================================================
    /// @name Consulta dos Jogadores
    // =========================================================
    ///@{

    /**
     * @brief Retorna referência para um jogador pelo ID.
     * @param idJogador ID do jogador.
     * @return Referência para o JogadorPife correspondente.
     */
    JogadorPife& consultarJogador(int idJogador);

    /**
     * @brief Retorna referência constante para um jogador pelo ID.
     * @param idJogador ID do jogador.
     * @return Referência constante para o JogadorPife correspondente.
     */
    const JogadorPife& consultarJogador(int idJogador) const;

    /**
     * @brief Retorna a mão de um jogador.
     * @param idJogador ID do jogador.
     * @return Referência constante para o vetor de cartas do jogador.
     */
    const std::vector<Carta>& consultarMao(int idJogador) const;

    ///@}

    // =========================================================
    /// @name Consulta da Mesa
    // =========================================================
    ///@{

    /**
     * @brief Retorna as cartas descartadas na mesa.
     * @return Referência constante para o vetor da mesa.
     */
    const std::vector<Carta>& consultarMesa() const;

    /**
     * @brief Retorna a carta vira da rodada.
     * @return Referência constante para a carta vira.
     */
    const Carta& consultarVira() const;

    ///@}
};

#endif