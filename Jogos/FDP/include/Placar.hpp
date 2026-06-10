#pragma once
#include <vector>
#include "Jogador.hpp"
#include "JogadorFDP.hpp"

/**
 * @brief Gerencia o placar e os resultados de uma partida do FDP.
 *
 * Responsável por calcular os resultados ao final de cada rodada,
 * aplicar dano aos jogadores que não cumpriram suas apostas e
 * exibir o estado atual do placar.
 */
class Placar {
public:
    /** @brief Construtor padrão. */
    Placar() = default;

    /** @brief Destrutor padrão. */
    ~Placar() = default;

    /**
     * @brief Exibe o placar atual de todos os jogadores.
     * @param jogadores Lista de jogadores participantes.
     */
    void exibirPlacar(std::vector<Jogador*> jogadores);

    /**
     * @brief Calcula e aplica os resultados ao fim de uma rodada.
     *
     * Compara as apostas com as vazas ganhas e aplica dano
     * aos jogadores que não cumpriram sua aposta.
     * @param jogadores Lista de jogadores participantes.
     */
    void calcularResultados(std::vector<Jogador*> jogadores);

    /**
     * @brief Conta quantos jogadores ainda estão vivos na partida.
     * @param jogadores Lista de jogadores participantes.
     * @return Número de jogadores com vidas restantes maior que zero.
     */
    int jogadoresVivos(std::vector<Jogador*> jogadores);
};