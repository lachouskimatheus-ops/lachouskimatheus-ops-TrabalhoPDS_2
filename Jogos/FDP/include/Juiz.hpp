#pragma once
#include <vector>
#include "Carta.hpp"

/**
 * @brief Interface abstrata para o árbitro de uma rodada do FDP.
 *
 * Define o contrato que todo juiz deve implementar: decidir o vencedor
 * de uma vaza com base nas cartas jogadas e na carta vira.
 * Permite diferentes variantes de regras via polimorfismo
 * (ex: JuizPaulista).
 */
class Juiz {
public:
    /**
     * @brief Decide o vencedor de uma vaza.
     *
     * Método puramente virtual — cada subclasse implementa as regras
     * da sua variante do jogo.
     *
     * @param cartasNaMesa Vetor com ponteiros para as cartas jogadas na vaza.
     * @param vira         Carta vira que define os trunfos da rodada.
     * @return Índice do jogador vencedor da vaza.
     */
    virtual int decidirVencedor(std::vector<Carta*> cartasNaMesa, Carta vira) = 0;

    /** @brief Destrutor virtual. Garante destruição correta via polimorfismo. */
    virtual ~Juiz() = default;
};