#pragma once
#include <string>
#include <vector>

class Carta; ///< Declaração antecipada para evitar inclusão circular

/**
 * @brief Classe base que representa um jogador genérico na coletânea de jogos.
 *
 * Define a interface comum a todos os jogadores: identidade, mão de cartas
 * e operações básicas. Serve como base para especializações como JogadorFDP.
 * Utiliza vetor de ponteiros para garantir unicidade das cartas e melhor
 * performance no acesso.
 */
class Jogador {
protected:
    int id_;                    ///< Identificador único do jogador
    std::string nome_;          ///< Nome do jogador
    std::vector<Carta*> mao_;   ///< Cartas na mão do jogador (ponteiros para cartas únicas)

public:
    /**
     * @brief Construtor.
     * @param id   Identificador único do jogador.
     * @param nome Nome do jogador.
     */
    Jogador(int id, std::string nome);

    /**
     * @brief Destrutor virtual. Garante destruição correta em hierarquias de herança.
     */
    virtual ~Jogador();

    /**
     * @brief Adiciona uma carta à mão do jogador.
     * @param novaCarta Ponteiro para a carta a ser recebida.
     */
    void receberCarta(Carta* novaCarta);

    /**
     * @brief Remove e retorna uma carta da mão do jogador.
     *
     * Método virtual para permitir comportamentos específicos em subclasses.
     * @param posicao Índice da carta na mão (0-based).
     * @return Ponteiro para a carta jogada.
     */
    virtual Carta* jogarCarta(int posicao);

    // =========================================================
    /// @name Getters
    // =========================================================
    ///@{

    /** @return Referência constante para o vetor de cartas na mão. */
    const std::vector<Carta*>& getMao() const { return mao_; }

    /**
     * @brief Retorna uma carta específica da mão pelo índice.
     * @param indice Índice da carta (0-based).
     * @return Ponteiro para a carta na posição indicada.
     */
    Carta* getCartaMao(int indice) { return mao_[indice]; }

    /** @return Nome do jogador. */
    std::string getNome() const;

    /** @return Identificador único do jogador. */
    int getId() const;

    /** @return Quantidade de cartas atualmente na mão. */
    int getQtdCartasMao() const;

    ///@}

    // =========================================================
    /// @name Setters
    // =========================================================
    ///@{

    /**
     * @brief Define o nome do jogador.
     * @param novoNome Novo nome a ser atribuído.
     */
    void setNome(std::string novoNome);

    ///@}
};