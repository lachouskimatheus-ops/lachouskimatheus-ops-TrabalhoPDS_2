#ifndef PONTUACAO_H
#define PONTUACAO_H

#include <string>

/**
 * @brief Eventos de pontuação que ocorrem durante o jogo.
 *
 * Cada valor representa uma ação do jogador que gera ou penaliza pontos.
 * O uso de `enum class` garante type safety, impedindo mistura acidental com inteiros.
 */
enum class EventoPontuacao {
    CavaParaColuna,     ///< Carta movida da cava (descarte) para uma coluna      (+5 pts)
    CavaParaFundacao,   ///< Carta movida da cava (descarte) para uma fundação     (+10 pts)
    ColunaParaFundacao, ///< Carta movida de uma coluna para uma fundação          (+10 pts)
    FundacaoParaColuna, ///< Carta retirada de uma fundação de volta para coluna   (-15 pts)
    VirarCarta,         ///< Carta de coluna virada para cima (revelada)           (+5 pts)
    ColunaParaColuna,   ///< Carta ou bloco movido entre colunas                   (+5 pts)
    PassarBaralho       ///< Cava reciclada (descarte reinserido na cava)          (-10 pts a partir da 2ª vez)
};

/**
 * @brief Gerencia a pontuação e o recorde persistente do jogo Paciência.
 *
 * Controla os pontos acumulados durante uma partida, aplica as regras de
 * pontuação para cada evento do jogo e persiste o recorde máximo em disco
 * no arquivo `record.txt`.
 */
class Pontuacao {
private:
    int pontos;       ///< Pontuação acumulada na partida atual
    int passadasCava; ///< Número de vezes que o baralho foi reciclado
    int record;       ///< Maior pontuação registrada (carregada do disco)

    /// @brief Caminho do arquivo onde o recorde é persistido.
    static constexpr const char* ARQUIVO_RECORD = "record.txt";

    /**
     * @brief Carrega o recorde salvo em disco para o membro `record`.
     *
     * Chamado automaticamente no construtor. Se o arquivo não existir,
     * inicializa o recorde em zero.
     */
    void carregarRecord();

public:

    /**
     * @brief Construtor. Inicializa pontos e passadas em zero e carrega o recorde do disco.
     */
    Pontuacao();

    // =========================================================
    /// @name Lógica de Negócio
    // =========================================================
    ///@{

    /**
     * @brief Aplica a variação de pontos correspondente a um evento do jogo.
     *
     * A pontuação nunca fica negativa — se o resultado for menor que zero,
     * é fixado em zero.
     * @param evento O evento ocorrido durante a partida.
     */
    void aplicar(EventoPontuacao evento);

    /**
     * @brief Reseta a pontuação e o contador de passadas para uma nova partida.
     *
     * Não altera o recorde salvo em disco.
     */
    void resetar();

    ///@}

    // =========================================================
    /// @name Persistência
    // =========================================================
    ///@{

    /**
     * @brief Salva a pontuação atual como recorde se for maior que o anterior.
     *
     * Grava o novo recorde no arquivo `record.txt`.
     * @return true se o recorde foi atualizado, false caso contrário.
     */
    bool salvarRecord();

    ///@}

    // =========================================================
    /// @name Getters
    // =========================================================
    ///@{

    /** @return Pontuação atual da partida. */
    int getPontos() const { return pontos; }

    /** @return Maior pontuação já registrada. */
    int getRecord() const { return record; }

    /** @return Número de vezes que o baralho foi reciclado na partida atual. */
    int getPassadasCava() const { return passadasCava; }

    ///@}

    // =========================================================
    /// @name Setters
    // =========================================================
    ///@{

    /**
     * @brief Define o número de passadas pela cava.
     *
     * Usado principalmente para restaurar o estado ao desfazer uma jogada.
     * @param p Novo valor de passadas.
     */
    void setPassadasCava(int p);

    /**
     * @brief Define a pontuação atual diretamente.
     *
     * Usado principalmente para restaurar o estado ao desfazer uma jogada.
     * @param p Novo valor de pontos.
     */
    void setPontos(int p);

    ///@}
};

#endif