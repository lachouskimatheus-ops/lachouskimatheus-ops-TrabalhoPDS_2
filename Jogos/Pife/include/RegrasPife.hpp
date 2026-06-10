#ifndef REGRAS_PIFE_HPP
#define REGRAS_PIFE_HPP

#include "Carta.hpp"
#include <vector>

/**
 * @brief Verifica se um índice é válido para um vetor de cartas.
 * @param mao    Vetor de cartas a verificar.
 * @param indice Índice a validar.
 * @return true se o índice está dentro dos limites do vetor.
 */
bool indiceValido(const std::vector<Carta>& mao, int indice);

/**
 * @brief Determina o valor do coringa com base na carta vira.
 *
 * O coringa é a carta com valor imediatamente acima da vira, mesmo naipe.
 * @param vira Carta vira da rodada.
 * @return Valor correspondente ao coringa.
 */
Valor valorDoCoringa(const Carta& vira);

/**
 * @brief Verifica se uma carta é coringa na rodada atual.
 * @param carta Carta a verificar.
 * @param vira  Carta vira que define o coringa.
 * @return true se a carta é coringa.
 */
bool cartaEhCoringa(const Carta& carta, const Carta& vira);

/**
 * @brief Verifica se três cartas formam uma trinca válida.
 *
 * Uma trinca é composta por três cartas de mesmo valor e naipes diferentes.
 * Coringas podem substituir qualquer carta.
 * @param mao  Mão do jogador.
 * @param i1   Índice da primeira carta.
 * @param i2   Índice da segunda carta.
 * @param i3   Índice da terceira carta.
 * @param vira Carta vira que define o coringa.
 * @return true se as três cartas formam uma trinca.
 */
bool trinca(const std::vector<Carta>& mao, int i1, int i2, int i3, const Carta& vira);

/**
 * @brief Verifica se três cartas formam uma sequência válida.
 *
 * Uma sequência é composta por três cartas do mesmo naipe com valores consecutivos.
 * Coringas podem substituir qualquer carta.
 * @param mao  Mão do jogador.
 * @param i1   Índice da primeira carta.
 * @param i2   Índice da segunda carta.
 * @param i3   Índice da terceira carta.
 * @param vira Carta vira que define o coringa.
 * @return true se as três cartas formam uma sequência.
 */
bool sequencia(const std::vector<Carta>& mao, int i1, int i2, int i3, const Carta& vira);

/**
 * @brief Verifica se três cartas formam uma combinação válida (trinca ou sequência).
 * @param mao  Mão do jogador.
 * @param i1   Índice da primeira carta.
 * @param i2   Índice da segunda carta.
 * @param i3   Índice da terceira carta.
 * @param vira Carta vira que define o coringa.
 * @return true se as três cartas formam trinca ou sequência.
 */
bool combinacaoValida(const std::vector<Carta>& mao, int i1, int i2, int i3, const Carta& vira);

/**
 * @brief Busca recursivamente se as cartas da mão podem ser agrupadas em combinações válidas.
 *
 * Usa backtracking para tentar todas as combinações possíveis de três cartas.
 * @param mao                 Cópia da mão a ser analisada (modificada durante a busca).
 * @param combinacoesFormadas Número de combinações válidas já encontradas.
 * @param vira                Carta vira que define o coringa.
 * @return true se todas as cartas podem ser agrupadas em combinações válidas.
 */
bool buscaCombinacao(std::vector<Carta> mao, int combinacoesFormadas, const Carta& vira);

/**
 * @brief Verifica se a mão de um jogador constitui uma vitória (pife).
 *
 * Uma mão vencedora é aquela onde todas as cartas podem ser agrupadas
 * em combinações válidas de três (trincas ou sequências), podendo
 * utilizar coringas.
 * @param mao  Mão do jogador a ser verificada.
 * @param vira Carta vira que define o coringa.
 * @return true se a mão é vencedora.
 */
bool verificaVitoria(const std::vector<Carta>& mao, const Carta& vira);

#endif