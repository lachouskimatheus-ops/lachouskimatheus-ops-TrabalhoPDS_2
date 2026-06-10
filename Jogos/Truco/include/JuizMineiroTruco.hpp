/**
 * @file JuizMineiroTruco.hpp
 * @brief Especialização de arbitragem para as regras do Truco Mineiro.
 */

#pragma once

#include "JuizTruco.hpp"

/**
 * @class JuizMineiroTruco
 * @brief Implementa a lógica tradicional do Truco Mineiro, onde as manilhas são fixas.
 *
 * No Truco Mineiro, a força das manilhas não depende do vira. Elas seguem a ordem fixa:
 * 4 de Paus (Zap), 7 de Copas, Ás de Espadas (Espadilha) e 7 de Ouros (Pica-fumo).
 */
class JuizMineiroTruco : public JuizTruco {
private:
    /**
     * @brief Atribui um peso numérico de força para uma carta com base no valor nominal mineiro.
     * @param carta Objeto Carta para avaliação.
     * @return Peso de força inteiro da carta.
     */
    int valorCarta(const Carta& carta) const;

    /**
     * @brief Calcula a força do naipe para desempate exclusivo de manilhas mineiras.
     * @param naipe Enum do Naipe a ser pesado.
     * @return Peso inteiro do naipe.
     */
    int forcaNaipe(Naipe naipe) const;

public:
    /**
     * @brief Avalia as cartas postas na mesa e decide o assento vencedor da queda conforme o regulamento mineiro.
     */
    int decidirVencedor(const std::vector<Carta*>& cartas, const Carta& vira, bool forcarVencedor = false) const override;
};