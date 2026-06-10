/**
 * @file Jogador_Truco.hpp
 * @brief Definição da classe Jogador_Truco que representa um participante da mesa de Truco.
 */

#pragma once

#include <string>
#include <vector>
#include "Carta.hpp"

/**
 * @class Jogador_Truco
 * @brief Gerencia a identidade, a mão de cartas e as ações locais de um jogador de Truco.
 *
 * Armazena ponteiros para as cartas recebidas a cada mão, permitindo que o participante 
 * jogue, limpe ou consulte as opções de manipulação da sua mão durante as quedas.
 */
class Jogador_Truco {
private:
    std::string nome_;           ///< Nome ou apelido de exibição do jogador.
    std::vector<Carta*> mao_;    ///< Vetor de ponteiros para as cartas atualmente na mão do jogador.

public:
    /**
     * @brief Construtor explícito que inicializa o jogador com um nome.
     * @param nome String contendo o nome do participante.
     */
    explicit Jogador_Truco(const std::string& nome);

    /**
     * @brief Adiciona uma nova carta à mão do jogador para o início da rodada.
     * @param novaCarta Ponteiro para o objeto Carta a ser recebido.
     */
    void receberCarta(Carta* novaCarta);

    /**
     * @brief Esvazia a mão do jogador, liberando a estrutura sem deletar a memória da Carta de forma global.
     */
    void limparMao();

    /**
     * @brief Remove e destaca uma carta da mão do jogador para ser posta na mesa.
     * @param indice Posição da carta na mão (0 a tamanho-1).
     * @return Ponteiro para a Carta selecionada que foi jogada.
     */
    Carta* jogarCarta(int indice);

    /**
     * @brief Obtém o nome do jogador.
     * @return Referência constante para a string do nome.
     */
    const std::string& getNome() const;

    /**
     * @brief Retorna uma referência constante para o vetor interno de ponteiros da mão.
     * @return Referência constante para std::vector<Carta*>.
     */
    const std::vector<Carta*>& getMao() const;

    /**
     * @brief Consulta a quantidade atual de cartas ainda restantes na mão.
     * @return Inteiro com o número de cartas (máximo 3).
     */
    int getQuantidadeCartas() const;
};