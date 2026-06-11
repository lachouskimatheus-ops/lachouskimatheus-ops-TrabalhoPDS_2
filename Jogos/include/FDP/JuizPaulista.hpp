#pragma once
#include "Juiz.hpp"

/**
 * @brief Implementação do juiz seguindo as regras paulistas do FDP.
 *
 * Concretiza a interface Juiz aplicando a hierarquia de forças das cartas
 * e dos naipes conforme as regras da variante paulista do jogo.
 */
class JuizPaulista : public Juiz {
public:
    /**
     * @brief Decide o vencedor da vaza pelas regras paulistas.
     *
     * Avalia as cartas jogadas considerando a carta vira para determinar
     * trunfos e aplica a hierarquia de forças dos naipes para desempate.
     *
     * @param cartasNaMesa Vetor com ponteiros para as cartas jogadas na vaza.
     * @param vira         Carta vira que define os trunfos da rodada.
     * @return Índice do jogador vencedor da vaza.
     */
    int decidirVencedor(std::vector<Carta*> cartasNaMesa, Carta vira) override;

    /**
     * @brief Retorna o peso do naipe conforme a hierarquia do FDP.
     *
     * Usado para desempate quando duas cartas têm o mesmo valor.
     * @param n Naipe a ser avaliado.
     * @return Peso numérico do naipe (maior = mais forte).
     */
    int obterPesoNaipeFDP(Naipe n);

    /**
     * @brief Retorna a força normal de uma carta pelo seu valor no FDP.
     *
     * Mapeia o valor numérico da carta para sua força relativa nas regras
     * paulistas, independente de trunfos.
     * @param valor Valor numérico da carta (1–13).
     * @return Força relativa da carta.
     */
    int obterForcaNormalFDP(int valor);
};