/**
 * @file SessaoWebSocket.hpp
 * @brief Definição da estrutura de dados de contexto para conexões WebSocket.
 */

#ifndef SESSAO_WEBSOCKET_HPP
#define SESSAO_WEBSOCKET_HPP

#include <string>

/**
 * @struct SessaoWebSocket
 * @brief Agrupa as informações de identificação associadas a uma ligação WebSocket ativa.
 *
 * Mapeia o canal de comunicação aberto, permitindo ao servidor saber instantaneamente
 * a qual sala o socket pertence, qual é o ID do jogador e o token que valida o canal.
 */
struct SessaoWebSocket {
    std::string idSala;         ///< String identificadora da sala onde a ligação está registada.
    int idJogador;              ///< ID do assento do jogador dono desta ligação.
    std::string tokenReconexao; ///< Token utilizado para autenticar a permanência na sessão.
};

#endif