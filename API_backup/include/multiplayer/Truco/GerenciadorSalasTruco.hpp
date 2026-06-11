/**
 * @file GerenciadorSalasTruco.hpp
 * @brief Definição da classe gerenciadora de salas para o jogo Truco.
 *
 * Orquestra o ciclo de vida global de todas as instâncias de SalaTruco no servidor,
 * permitindo a criação centralizada, consulta e destruição de salas ativas.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include "multiplayer/Truco/SalaTruco.hpp"

/**
 * @class GerenciadorSalasTruco
 * @brief Gestor estático responsável pela coleção e ciclo de vida das salas de Truco.
 *
 * Utiliza um mapa indexado pelo ID alfanumérico da sala associado a ponteiros
 * únicos (`std::unique_ptr`), garantindo que a memória de cada sala seja limpa
 * automaticamente quando removida.
 */
class GerenciadorSalasTruco {
private:
    /**
     * @brief Tabela/mapa que associa o ID da sala à sua respetiva instância única de SalaTruco.
     */
    static std::map<std::string, std::unique_ptr<SalaTruco>> salas_;

    /**
     * @brief Gera de forma pseudo-aleatória um código identificador único para novas salas.
     * @return String contendo o ID alfanumérico gerado.
     */
    static std::string gerarIdSala();

public:
    /**
     * @brief Instancia uma nova SalaTruco (Paulista ou Mineiro) e adiciona-a ao mapa.
     * @param tipo O regulamento do Truco a ser jogado (TipoTruco::Paulista ou TipoTruco::Mineiro).
     * @param maxJogadores Lotação máxima de assentos permitida na sala.
     * @return Ponteiro bruto para a SalaTruco recém-criada.
     */
    static SalaTruco* criarSala(TipoTruco tipo, int maxJogadores);

    /**
     * @brief Procura uma sala ativa através do seu código identificador único.
     * @param idSala Código alfanumérico da sala pretendida.
     * @return Ponteiro para a SalaTruco correspondente, ou nullptr se não for localizada.
     */
    static SalaTruco* obterSala(const std::string& idSala);

    /**
     * @brief Verifica se um determinado ID de sala já está registado no sistema.
     * @param idSala Código identificador a ser testado.
     * @return true se a sala existir, false caso contrário.
     */
    static bool salaExiste(const std::string& idSala);

    /**
     * @brief Remove uma sala do mapa e liberta automaticamente toda a sua memória associada.
     * @param idSala Código identificador da sala a ser eliminada.
     * @return true se a sala foi encontrada e removida com sucesso, false caso contrário.
     */
    static bool removerSala(const std::string& idSala);
};