/**
 * @file GerenciadorSalasFDP.hpp
 * @brief Definição da classe gerenciadora de salas para o jogo FDP.
 *
 * Este ficheiro contém a declaração do GerenciadorSalasFDP, responsável por
 * criar, obter, verificar a existência e remover instâncias de SalaFDP
 * de forma global no servidor.
 */

#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "multiplayer/FDP/SalaFDP.hpp"

/**
 * @class GerenciadorSalasFDP
 * @brief Gestor estático e centralizado para o ciclo de vida das salas do jogo FDP.
 *
 * Controla o mapeamento de todas as salas ativas através dos seus identificadores únicos,
 * garantindo exclusão mútua (thread-safety) nas operações concorrentes de criação e remoção.
 */
class GerenciadorSalasFDP {
private:
    /**
     * @brief Mapa que associa o ID alfanumérico da sala ao seu respetivo ponteiro partilhado.
     */
    static std::map<std::string, std::shared_ptr<SalaFDP>> salas_;

    /**
     * @brief Mutex utilizado para garantir a segurança entre threads ao manipular o mapa de salas.
     */
    static std::mutex mutexSalas_;

    /**
     * @brief Gera um identificador único aleatório para ser utilizado numa nova sala.
     * @return Uma string correspondente ao ID gerado.
     */
    static std::string gerarIdSala();

public:
    /**
     * @brief Cria e regista uma nova instância de SalaFDP no servidor.
     * @param maxJogadores Número máximo de jogadores permitidos na sala.
     * @return Um ponteiro partilhado (std::shared_ptr) para a SalaFDP criada.
     */
    static std::shared_ptr<SalaFDP> criarSala(int maxJogadores);

    /**
     * @brief Obtém o ponteiro de uma sala ativa a partir do seu ID.
     * @param idSala O identificador único da sala.
     * @return Um std::shared_ptr para a sala encontrada, ou nullptr caso não exista.
     */
    static std::shared_ptr<SalaFDP> obterSala(const std::string& idSala);

    /**
     * @brief Verifica se uma determinada sala existe no mapa de registos.
     * @param idSala O identificador único da sala.
     * @return true se a sala existir, false caso contrário.
     */
    static bool salaExiste(const std::string& idSala);

    /**
     * @brief Remove permanentemente uma sala do servidor através do seu ID.
     * @param idSala O identificador único da sala a ser removida.
     * @return true se a sala foi removida com sucesso, false se não foi encontrada.
     */
    static bool removerSala(const std::string& idSala);

    /**
     * @brief Remove uma sala do servidor apenas se ela não contiver jogadores.
     * @param idSala O identificador único da sala.
     * @return true se a sala estava vazia e foi removida, false caso contrário.
     */
    static bool removerSalaSeVazia(const std::string& idSala);

    /**
     * @brief Lista os identificadores de todas as salas atualmente abertas no servidor.
     * @return Um vetor contendo as strings dos IDs de todas as salas ativas.
     */
    static std::vector<std::string> listarIdsSalas();
};