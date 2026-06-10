/**
 * @file JogadorConectado.hpp
 * @brief Definição da estrutura que monitoriza o estado de ligação de um cliente.
 */

#ifndef JOGADOR_CONECTADO_HPP
#define JOGADOR_CONECTADO_HPP

#include <string>

/**
 * @struct JogadorConectado
 * @brief Armazena o estado de atividade de rede e credenciais de reconexão de um utilizador.
 *
 * Utilizada pelos gestores de salas para controlar se um assento está ocupado, se o
 * utilizador caiu por instabilidade de rede ou se possui um token válido para reclamar a vaga.
 */
struct JogadorConectado {
    int idJogador;              ///< ID numérico identificador do assento atribuído na sala.
    bool conectado;             ///< Flag booleana que indica se a sessão WebSocket está ativa.
    std::string tokenReconexao; ///< Hash/Token único que valida a identidade do cliente ao reatar a ligação.
};

#endif