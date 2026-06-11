#pragma once

#include "Jogador.hpp"
#include "Carta.hpp"

#include <string>
#include <vector>

/**
 * @brief Representa um jogador do Blackjack/21.
 *
 * O jogador possui uma mão de cartas herdada da classe Jogador e adiciona
 * comportamentos específicos do 21, como cálculo de pontuação com Ás valendo
 * 1 ou 11, controle de parada e verificação de estouro.
 */
class Jogador21 : public Jogador {
private:
    int pontuacaoAtual_;   ///< Pontuação calculada da mão.
    bool parou_;           ///< Indica se o jogador decidiu parar.

public:
    /**
     * @brief Construtor do jogador de Blackjack.
     * @param id Identificador do jogador.
     * @param nome Nome do jogador.
     */
    Jogador21(int id, const std::string& nome);

    /**
     * @brief Destrutor padrão.
     */
    ~Jogador21() override = default;

    /**
     * @brief Calcula a pontuação atual da mão.
     *
     * Regras usadas:
     * - Ás vale 11 inicialmente.
     * - Se a mão passar de 21, cada Ás pode passar a valer 1.
     * - J, Q e K valem 10.
     *
     * @return Pontuação final considerando ajuste de Ás.
     */
    int calcularPontuacao();

    /**
     * @brief Verifica se o jogador passou de 21.
     * @return true se a pontuação atual for maior que 21.
     */
    bool estourou();

    /**
     * @brief Retorna a última pontuação calculada.
     * @return Pontuação atual.
     */
    int getPontuacao() const;

    /**
     * @brief Marca que o jogador decidiu parar.
     */
    void parar();

    /**
     * @brief Verifica se o jogador já parou.
     * @return true se o jogador já parou.
     */
    bool parou() const;

    /**
     * @brief Reseta o estado do jogador para uma nova rodada.
     *
     * Remove as cartas da mão, zera a pontuação e libera o estado de parada.
     */
    void prepararNovaRodada();

    /**
     * @brief Retorna a mão do jogador.
     * @return Vetor constante de ponteiros para cartas.
     */
    const std::vector<Carta*>& verMao() const;
};
