/**
 * @file GerenciadorSalasPoker.hpp
 * @brief Definição da classe gerenciadora de salas para o jogo Poker.
 *
 * Este ficheiro gerencia o ciclo de vida global de todas as instâncias de SalaPoker
 * ativas no servidor, utilizando mapeamento por ID alfanumérico.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include "multiplayer/Poker/SalaPoker.hpp"

/**
 * @class GerenciadorSalasPoker
 * @brief Gestor centralizado e estático para a coleção de salas de Poker.
 *
 * Utiliza ponteiros únicos (`std::unique_ptr`) para garantir o encapsulamento e
 * a desalocação segura de cada SalaPoker quando esta é removida do sistema.
 */
class GerenciadorSalasPoker {
private:
    /**
     * @brief Tabela/mapa que associa o ID da sala à sua respetiva instância única de SalaPoker.
     */
    static std::map<std::string, std::unique_ptr<SalaPoker>> salas_;

    /**
     * @brief Gera de forma pseudo-aleatória um código identificador único para novas salas.
     * @return String contendo o ID gerado.
     */
    static std::string gerarIdSala();

public:
    /**
     * @brief Cria uma nova SalaPoker e adiciona-a ao mapa de registos.
     * @param maxJogadores Lotação máxima de assentos permitida na sala.
     * @param modo O modo de jogo do Poker (ex: ContraComputador ou Multiplayer).
     * @return Ponteiro bruto para a SalaPoker recém-criada.
     */
    static SalaPoker* criarSala(int maxJogadores, ModoPoker modo);

    /**
     * @brief Procura uma sala ativa através do seu código identificador.
     * @param idSala Código alfanumérico da sala pretendida.
     * @return Ponteiro para a SalaPoker correspondente, ou nullptr se não for encontrada.
     */
    static SalaPoker* obterSala(const std::string& idSala);

    /**
     * @brief Verifica se um determinado ID de sala já se encontra registado no sistema.
     * @param idSala Código identificador a ser verificado.
     * @return true se a sala existir, false caso contrário.
     */
    static bool salaExiste(const std::string& idSala);

    /**
     * @brief Remove uma sala do mapa e liberta automaticamente a memória alocada.
     * @param idSala Código identificador da sala a ser eliminada.
     * @return true se a sala foi localizada e removida com sucesso, false caso contrário.
     */
    static bool removerSala(const std::string& idSala);

    /**
     * @brief Remove uma sala do servidor apenas se esta não contiver jogadores ativos.
     * @param idSala Código identificador da sala.
     * @return true se a sala estava vazia e foi removida, false caso contrário.
     */
    static bool removerSalaSeVazia(const std::string& idSala);
};