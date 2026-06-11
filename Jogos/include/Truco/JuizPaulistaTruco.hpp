/**
 * @file JuizPaulistaTruco.hpp
 * @brief Especialização de arbitragem para as regras do Truco Paulista.
 */

#pragma once

#include "JuizTruco.hpp"

/**
 * @class JuizPaulistaTruco
 * @brief Implementa as regras do Truco Paulista, caracterizado por manilhas variáveis.
 *
 * A força das manilhas é ditada dinamicamente pela carta definida como "Vira" na rodada. 
 * A manilha será sempre o valor imediatamente subsequente ao do Vira (ex: se o Vira for 4, a manilha será 5).
 */
class JuizPaulistaTruco : public JuizTruco {
private:
    /**
     * @brief Avalia o peso de uma carta considerando se ela atua ou não como manilha dinâmica.
     * @param carta Objeto Carta avaliado.
     * @param vira O Vira da rodada atual.
     * @return Peso numérico de força.
     */
    int valorCarta(const Carta& carta, const Carta& vira) const;

    /**
     * @brief Define a força hierárquica clássica dos naipes (Paus > Copas > Espadas > Ouros) para desempatar manilhas.
     * @param naipe Enum do Naipe avaliado.
     * @return Peso inteiro do naipe.
     */
    int forcaNaipe(Naipe naipe) const;

    /**
     * @brief Executa o cálculo aritmético cíclico para encontrar o valor numérico da manilha com base no vira.
     * @param valorVira Inteiro representando o valor nominal do Vira.
     * @return Inteiro com o valor nominal que a manilha assume.
     */
    int calcularValorManilha(int valorVira) const;

public:
    /**
     * @brief Avalia as cartas postas na mesa e decide o assento vencedor da queda conforme o regulamento paulista.
     */
    int decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor = false) const override;
};