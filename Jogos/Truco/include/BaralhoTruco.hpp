/**
 * @file BaralhoTruco.hpp
 * @brief Definição da classe BaralhoTruco especializada para as regras do jogo de Truco.
 */

#pragma once

#include "Baralho.hpp"

/**
 * @class BaralhoTruco
 * @brief Subclasse de Baralho que implementa o baralho limpo (ou "indiscreto") do Truco.
 *
 * Remove as cartas de valor 8, 9, 10 e os Coringas do conjunto tradicional de 52 cartas, 
 * restando apenas as 40 cartas regulamentares utilizadas nas modalidades Mineira e Paulista.
 */
class BaralhoTruco : public Baralho {
public:
    /**
     * @brief Construtor padrão. Inicializa a estrutura base de um baralho de Truco vazio.
     */
    BaralhoTruco();

    /**
     * @brief Inicializa e molda o vetor interno especificamente com as 40 cartas válidas do Truco.
     * * Sobrescreve o método virtual da classe base para aplicar a regra de exclusão 
     * das cartas que não jogam (8, 9 e 10).
     */
    void inicializar() override;

    /**
     * @brief Retira e retorna um ponteiro para a carta do topo do baralho.
     * @return Ponteiro para a Carta retirada do topo, ou nullptr se estiver vazio.
     */
    Carta* puxarCarta();
};