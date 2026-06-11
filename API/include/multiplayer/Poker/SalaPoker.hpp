/**
 * @file SalaPoker.hpp
 * @brief Definição da classe SalaPoker e da estrutura de conexões de rede associada.
 *
 * Acopla a lógica central do motor de Poker ao sistema multiplayer,
 * gerenciando jogadores, reconexões e sincronização do estado da partida.
 */

#pragma once

#include <string>
#include <vector>

#include "crow_all.h"
#include "json.hpp"

#include "coreAPI/SalaBase.hpp"
#include "Poker/MesaPoker.hpp"

using json = nlohmann::json;

/**
 * @struct ConexaoPoker
 * @brief Associa um jogador da sala à sua conexão WebSocket ativa.
 */
struct ConexaoPoker {
    int idJogador;                        ///< Identificador do jogador na sala.
    crow::websocket::connection* conexao; ///< Ponteiro para a conexão WebSocket.
};

/**
 * @class SalaPoker
 * @brief Gerencia uma sala multiplayer de Poker.
 *
 * Mantém a instância de MesaPoker, as conexões dos jogadores e o estado
 * de inicialização da partida.
 */
class SalaPoker : public SalaBase {
private:
    MesaPoker jogo_;                     ///< Motor lógico da partida.
    std::vector<ConexaoPoker> conexoes_; ///< Conexões WebSocket ativas.
    bool partidaIniciada_;               ///< Indica se a partida já começou.

    /**
     * @brief Procura a conexão associada a um jogador.
     * @param idJogador Identificador do jogador.
     * @return Ponteiro para o registro ou nullptr quando não encontrado.
     */
    ConexaoPoker* buscarConexaoDoJogador(int idJogador);

    /**
     * @brief Procura a conexão associada a um jogador para consulta.
     * @param idJogador Identificador do jogador.
     * @return Ponteiro constante para o registro ou nullptr.
     */
    const ConexaoPoker* buscarConexaoDoJogador(int idJogador) const;

    /**
     * @brief Converte uma fase do Poker em texto.
     * @param fase Fase que será convertida.
     * @return Representação textual da fase.
     */
    static std::string faseParaString(FasePoker fase);

    /**
     * @brief Converte um modo de Poker em texto.
     * @param modo Modo que será convertido.
     * @return Representação textual do modo.
     */
    static std::string modoParaString(ModoPoker modo);

public:
    /**
     * @brief Constrói uma sala de Poker.
     * @param idSala Identificador da sala.
     * @param maxJogadores Quantidade máxima de jogadores.
     * @param modo Modo de funcionamento da partida.
     */
    SalaPoker(const std::string& idSala, int maxJogadores, ModoPoker modo);

    /**
     * @brief Adiciona um jogador à sala.
     * @param conexao Conexão WebSocket do jogador.
     * @param tokenReconexao Token utilizado para reconexão.
     * @param nome Nome de exibição do jogador.
     * @return Identificador do jogador ou -1 em caso de falha.
     */
    int adicionarJogador(crow::websocket::connection* conexao,
                         const std::string& tokenReconexao,
                         const std::string& nome);

    /**
     * @brief Reconecta um jogador por meio de seu token.
     * @param conexao Nova conexão WebSocket.
     * @param tokenReconexao Token de reconexão.
     * @return Identificador do jogador ou -1 em caso de falha.
     */
    int reconectarJogador(crow::websocket::connection* conexao,
                          const std::string& tokenReconexao);

    /**
     * @brief Remove uma conexão da sala.
     * @param conexao Conexão que será removida.
     * @return true quando removida; false caso contrário.
     */
    bool removerConexao(crow::websocket::connection* conexao);

    /**
     * @brief Obtém o jogador associado a uma conexão.
     * @param conexao Conexão consultada.
     * @return Identificador do jogador ou -1.
     */
    int obterIdJogador(crow::websocket::connection* conexao) const;

    /**
     * @brief Obtém a conexão ativa de um jogador.
     * @param idJogador Identificador do jogador.
     * @return Ponteiro para a conexão ou nullptr.
     */
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    /**
     * @brief Verifica se uma conexão pertence à sala.
     * @param conexao Conexão consultada.
     * @return true quando pertencer; false caso contrário.
     */
    bool possuiConexao(crow::websocket::connection* conexao) const;

    /**
     * @brief Verifica se a sala pode receber outro jogador.
     * @return true quando houver vaga; false caso contrário.
     */
    bool podeReceberNovoJogador() const;

    /**
     * @brief Verifica se um token pode ser usado para reconexão.
     * @param tokenReconexao Token consultado.
     * @return true quando o token existir; false caso contrário.
     */
    bool podeReconectar(const std::string& tokenReconexao) const;

    /**
     * @brief Inicia a partida quando todos os jogadores necessários estão conectados.
     * @return true quando iniciada; false caso contrário.
     */
    bool iniciarPartida();

    /**
     * @brief Verifica se a partida já começou.
     * @return true quando iniciada; false caso contrário.
     */
    bool partidaIniciada() const;

    /**
     * @brief Confirma a troca de cartas de um jogador.
     * @param idJogador Identificador do jogador.
     * @param indices Índices das cartas escolhidas.
     * @return true quando a troca for aceita; false caso contrário.
     */
    bool confirmarTroca(int idJogador, const std::vector<int>& indices);

    /**
     * @brief Inicia uma nova rodada.
     * @return true quando iniciada; false caso contrário.
     */
    bool iniciarNovaRodada();

    /**
     * @brief Gera o estado da partida para um jogador.
     * @param idJogador Identificador do jogador solicitante.
     * @return Objeto JSON contendo o estado da sala.
     */
    json gerarJson(int idJogador) const;

    /**
     * @brief Retorna o motor lógico da partida.
     * @return Referência para MesaPoker.
     */
    MesaPoker& jogo();

    /**
     * @brief Retorna o motor lógico da partida para consulta.
     * @return Referência constante para MesaPoker.
     */
    const MesaPoker& jogo() const;

    /**
     * @brief Retorna as conexões da sala.
     * @return Referência para o vetor de conexões.
     */
    std::vector<ConexaoPoker>& conexoes();

    /**
     * @brief Retorna as conexões da sala para consulta.
     * @return Referência constante para o vetor de conexões.
     */
    const std::vector<ConexaoPoker>& conexoes() const;
};