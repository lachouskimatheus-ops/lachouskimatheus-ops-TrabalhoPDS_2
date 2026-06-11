/**
 * @file SalaTruco.hpp
 * @brief Definição da classe SalaTruco e do encapsulamento das conexões de rede do jogo.
 *
 * Integra a lógica central do Truco, o juiz responsável pela variante escolhida
 * e as conexões WebSocket utilizadas na partida multiplayer.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "crow_all.h"

#include "coreAPI/SalaBase.hpp"

#include "Truco/BaralhoTruco.hpp"
#include "Truco/Jogador_Truco.hpp"
#include "Truco/JuizTruco.hpp"
#include "Truco/Truco.hpp"

/**
 * @enum TipoTruco
 * @brief Define as variações de regras do Truco aceitas pelo sistema.
 */
enum class TipoTruco {
    Paulista, ///< Utiliza manilhas definidas a partir da carta vira.
    Mineiro   ///< Utiliza as manilhas fixas do Truco Mineiro.
};

/**
 * @struct ConexaoTruco
 * @brief Associa um jogador a uma conexão WebSocket ativa.
 */
struct ConexaoTruco {
    int idJogador;                        ///< Identificador do jogador na sala.
    crow::websocket::connection* conexao; ///< Conexão WebSocket ativa.
};

/**
 * @class SalaTruco
 * @brief Gerencia uma sala multiplayer de Truco.
 *
 * A classe mantém o baralho, o juiz, a instância principal do jogo,
 * os jogadores e as conexões WebSocket associadas à sala.
 */
class SalaTruco : public SalaBase {
private:
    TipoTruco tipo_;                                             ///< Variante de Truco utilizada.
    BaralhoTruco baralho_;                                       ///< Baralho utilizado na partida.
    std::unique_ptr<JuizTruco> juiz_;                            ///< Juiz responsável pela comparação das cartas.
    std::unique_ptr<Truco> jogo_;                                ///< Instância principal da lógica do Truco.
    std::vector<std::unique_ptr<Jogador_Truco>> jogadoresTruco_; ///< Jogadores pertencentes à sala.
    std::vector<ConexaoTruco> conexoes_;                         ///< Conexões WebSocket ativas.
    bool partidaIniciada_;                                      ///< Indica se a partida já começou.

    /**
     * @brief Procura a conexão associada a um jogador.
     * @param idJogador Identificador do jogador.
     * @return Ponteiro para o registro ou nullptr quando não encontrado.
     */
    ConexaoTruco* buscarConexaoDoJogador(int idJogador);

    /**
     * @brief Procura a conexão associada a um jogador para consulta.
     * @param idJogador Identificador do jogador.
     * @return Ponteiro constante para o registro ou nullptr quando não encontrado.
     */
    const ConexaoTruco* buscarConexaoDoJogador(int idJogador) const;

public:
    /**
     * @brief Constrói uma sala de Truco.
     * @param idSala Código identificador da sala.
     * @param tipo Variante de Truco escolhida.
     * @param maxJogadores Quantidade máxima de jogadores.
     */
    SalaTruco(const std::string& idSala, TipoTruco tipo, int maxJogadores);

    /**
     * @brief Adiciona um jogador à sala.
     * @param conexao Conexão WebSocket do jogador.
     * @param tokenReconexao Token utilizado para reconexão.
     * @param nome Nome do jogador.
     * @param equipe Equipe escolhida, sendo 1 ou 2.
     * @return Identificador do jogador ou -1 em caso de falha.
     */
    int adicionarJogador(crow::websocket::connection* conexao,
                         const std::string& tokenReconexao,
                         const std::string& nome,
                         int equipe);

    /**
     * @brief Reconecta um jogador usando seu token.
     * @param conexao Nova conexão WebSocket.
     * @param tokenReconexao Token de reconexão.
     * @return Identificador do jogador ou -1 em caso de falha.
     */
    int reconectarJogador(crow::websocket::connection* conexao,
                          const std::string& tokenReconexao);

    /**
     * @brief Remove uma conexão ativa da sala.
     * @param conexao Conexão que será removida.
     * @return true quando removida; false caso contrário.
     */
    bool removerConexao(crow::websocket::connection* conexao);

    /**
     * @brief Inicializa a partida quando todos os jogadores estiverem conectados.
     * @return true quando a partida estiver iniciada; false caso contrário.
     */
    bool iniciarPartida();

    /**
     * @brief Verifica se a partida já começou.
     * @return true quando iniciada; false caso contrário.
     */
    bool partidaIniciada() const;

    /**
     * @brief Verifica se a sala pode receber outro jogador.
     * @return true quando houver vaga; false caso contrário.
     */
    bool podeReceberNovoJogador() const;

    /**
     * @brief Verifica se um token pode ser utilizado para reconexão.
     * @param tokenReconexao Token informado pelo cliente.
     * @return true quando o token existir; false caso contrário.
     */
    bool podeReconectar(const std::string& tokenReconexao) const;

    /**
     * @brief Verifica se uma equipe ainda possui vaga.
     * @param equipe Equipe consultada.
     * @return true quando houver vaga; false caso contrário.
     */
    bool equipeDisponivel(int equipe) const;

    /**
     * @brief Obtém o jogador associado a uma conexão.
     * @param conexao Conexão WebSocket consultada.
     * @return Identificador do jogador ou -1.
     */
    int obterIdJogador(crow::websocket::connection* conexao) const;

    /**
     * @brief Retorna a quantidade de jogadores em uma equipe.
     * @param equipe Equipe consultada.
     * @return Quantidade de jogadores.
     */
    int quantidadeNaEquipe(int equipe) const;

    /**
     * @brief Obtém a conexão ativa de um jogador.
     * @param idJogador Identificador do jogador.
     * @return Conexão ativa ou nullptr.
     */
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    /**
     * @brief Verifica se uma conexão pertence à sala.
     * @param conexao Conexão consultada.
     * @return true quando pertencer; false caso contrário.
     */
    bool possuiConexao(crow::websocket::connection* conexao) const;

    /**
     * @brief Retorna a variante de Truco utilizada.
     * @return Variante configurada para a sala.
     */
    TipoTruco tipo() const;

    /**
     * @brief Retorna a instância principal do jogo.
     * @return Referência para a instância de Truco.
     */
    Truco& jogo();

    /**
     * @brief Retorna a instância principal do jogo para consulta.
     * @return Referência constante para a instância de Truco.
     */
    const Truco& jogo() const;

    /**
     * @brief Retorna um jogador de Truco.
     * @param idJogador Identificador do jogador.
     * @return Ponteiro para o jogador ou nullptr.
     */
    Jogador_Truco* jogadorTruco(int idJogador);

    /**
     * @brief Retorna um jogador de Truco para consulta.
     * @param idJogador Identificador do jogador.
     * @return Ponteiro constante para o jogador ou nullptr.
     */
    const Jogador_Truco* jogadorTruco(int idJogador) const;

    /**
     * @brief Retorna as conexões da sala.
     * @return Referência para o vetor de conexões.
     */
    std::vector<ConexaoTruco>& conexoes();

    /**
     * @brief Retorna as conexões da sala para consulta.
     * @return Referência constante para o vetor de conexões.
     */
    const std::vector<ConexaoTruco>& conexoes() const;
};