#pragma once
#include "Baralho.hpp"

/**
 * @brief Baralho especializado para o jogo FDP (Fodinha de Paus).
 *
 * Herda de Baralho e sobrescreve a inicialização para montar o baralho
 * com as regras específicas do FDP, que utiliza um conjunto de cartas
 * diferente do baralho padrão de 52 cartas.
 */
class BaralhoSujo : public Baralho {
public:
    /** @brief Construtor. Inicializa o baralho com as cartas do FDP. */
    BaralhoSujo();

    /** @brief Destrutor virtual. Garante a destruição correta via polimorfismo. */
    ~BaralhoSujo() override = default;

    /**
     * @brief Inicializa o baralho com o conjunto de cartas do FDP.
     *
     * Sobrescreve o método da classe base para montar um baralho
     * com as regras e cartas específicas do jogo FDP.
     */
    void inicializar() override;
};