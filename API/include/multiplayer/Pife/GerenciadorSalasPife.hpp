/**
 * @file GerenciadorSalasPife.hpp
 * @brief Definição da classe gerenciadora de salas para o jogo Pife.
 *
 * Controla o ciclo de vida global das instâncias de SalaPife no servidor,
 * permitindo criação, busca, verificação e remoção.
 */

#ifndef GERENCIADOR_SALAS_PIFE_HPP
#define GERENCIADOR_SALAS_PIFE_HPP

#include <map>
#include <memory>
#include <string>

#include "multiplayer/Pife/SalaPife.hpp"

/**
 * @class GerenciadorSalasPife
 * @brief Gestor centralizado para gerenciar a coleção de salas do jogo Pife.
 *
 * Utiliza ponteiros únicos (std::unique_ptr) dentro de um mapa indexado pelo ID
 * alfanumérico da sala para garantir a posse e desalocação correta da memória.
 */
class GerenciadorSalasPife {
private:
    /**
     * @brief Tabela hash/mapa que associa o ID da sala à sua respectiva instância única de SalaPife.
     */
    static std::map<std::string, std::unique_ptr<SalaPife>> salas_;

    /**
     * @brief Gera de forma pseudo-aleatória um código identificador único para novas salas.
     * @return String contendo o ID gerado.
     */
    static std::string gerarIdSala();

public:
    /**
     * @brief Cria uma nova SalaPife e a armazena no mapa de salas ativas.
     * @param maxJogadores Limite de jogadores permitido para a nova sala.
     * @return Ponteiro bruto para a SalaPife recém-criada.
     */
    static SalaPife* criarSala(int maxJogadores);

    /**
     * @brief Busca uma sala ativa a partir de seu identificador único.
     * @param idSala Código alfanumérico da sala desejada.
     * @return Ponteiro para a SalaPife correspondente, ou nullptr se não for localizada.
     */
    static SalaPife* obterSala(const std::string& idSala);

    /**
     * @brief Verifica se um ID de sala específico já está registrado no sistema.
     * @param idSala Código identificador a ser testado.
     * @return true se a sala existir, false caso contrário.
     */
    static bool salaExiste(const std::string& idSala);

    /**
     * @brief Remove uma sala do mapa e libera automaticamente a sua memória associada.
     * @param idSala Código identificador da sala a ser excluída.
     * @return true se a sala foi encontrada e removida, false caso contrário.
     */
    static bool removerSala(const std::string& idSala);
};

#endif