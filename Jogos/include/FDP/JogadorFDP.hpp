#pragma once
#include "Jogador.hpp"

/**
 * @brief Especialização de Jogador para o jogo FDP (Fodinha de Paus).
 *
 * Adiciona os atributos e comportamentos específicos do FDP: sistema de vidas,
 * apostas por rodada e contagem de vazas ganhas.
 */
class JogadorFDP : public Jogador {
private:
    int vidas_;         ///< Número de vidas restantes do jogador
    int aposta_atual_;  ///< Aposta feita pelo jogador na rodada atual (-1 = ainda não apostou)
    int vezes_ganhas_;  ///< Número de vazas ganhas na rodada atual

public:
    /**
     * @brief Construtor.
     * @param id       Identificador único do jogador.
     * @param nome     Nome do jogador.
     * @param vidas_ini Número inicial de vidas (padrão: 5).
     */
    JogadorFDP(int id, std::string nome, int vidas_ini = 5);

    /**
     * @brief Registra a aposta do jogador para a rodada atual.
     * @param valor_aposta Número de vazas que o jogador aposta ganhar.
     */
    void fazerAposta(int valor_aposta);

    /** @brief Incrementa o contador de vazas ganhas ao vencer uma vaza. */
    void registrarGanhador();

    /** @brief Aplica dano ao jogador (reduz vidas) ao perder uma rodada. */
    void dano();

    /** @brief Reseta aposta e vazas ganhas para preparar uma nova rodada. */
    void prepararNovaRodada();

    /** @brief Ordem dos jogadores na vaza atual (usado para desempate). */
    std::vector<int> ordemJogadoresDaVaza_;

    // =========================================================
    /// @name Getters
    // =========================================================
    ///@{

    /** @return Número de vidas restantes. */
    int getVidas() const;

    /** @return Aposta atual do jogador (-1 se ainda não apostou). */
    int getAposta() const;

    /** @return Número de vazas ganhas na rodada atual. */
    int getVezesGanhas() const;

    ///@}

    // =========================================================
    /// @name Setters
    // =========================================================
    ///@{

    /**
     * @brief Define a aposta do jogador diretamente.
     * @param aposta Valor da aposta.
     */
    void setAposta(int aposta) { aposta_atual_ = aposta; }

    /** @brief Incrementa o contador de vazas feitas em uma unidade. */
    void adicionarVazaFeita() { vezes_ganhas_++; }

    /**
     * @brief Define o número de vidas do jogador.
     * @param novasVidas Novo valor de vidas.
     */
    void setVidas(int novasVidas);

    /**
     * @brief Define o nome do jogador.
     * @param novoNome Novo nome.
     */
    void setNome(const std::string& novoNome);

    ///@}
};