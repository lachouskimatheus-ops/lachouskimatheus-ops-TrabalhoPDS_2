/**
 * @file JuizTruco.hpp
 * @brief Interface abstrata para as regras de avaliação e arbitragem do Truco.
 */

#pragma once

#include <vector>
#include "Carta.hpp"

/**
 * @class JuizTruco
 * @brief Classe base abstrata responsável por definir os critérios de vitória de uma queda.
 *
 * Fornece a interface polimórfica necessária para tratar o cálculo de forças de cartas
 * e manilhas que diferem entre as variações regionais do jogo de Truco.
 */
class JuizTruco {
public:
    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~JuizTruco() = default;

    /**
     * @brief Método puramente virtual para decidir o vencedor de uma queda na mesa.
     * @param cartas Vetor de ponteiros das cartas jogadas na queda, ordenadas pelos IDs dos jogadores.
     * @param vira A carta aberta como "Vira" no centro da mesa.
     * @param forcarVencedor Flag opcional para desempates forçados pelas regras.
     * @return ID do índice do jogador vencedor da queda (ou código de empate regulamentar).
     */
    virtual int decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor = false) const = 0;
};