/**
 * @file SalaPife.hpp
 * @brief Especialização de sala multiplayer para gerenciar a lógica e conexões do jogo Pife.
 */

#ifndef SALA_PIFE_HPP
#define SALA_PIFE_HPP

#include <string>
#include <vector>

#include "crow_all.h"

#include "coreAPI/SalaBase.hpp"
#include "Pife.hpp"

/**
 * @struct ConexaoPife
 * @brief Estrutura de pareamento que vincula um ID de jogador ao seu ponteiro de conexão WebSocket.
 */
struct ConexaoPife {
    int idJogador;                        ///< ID numérico identificador do assento alocado.
    crow::websocket::connection* conexao; ///< Ponteiro para a conexão WebSocket associada do Crow.
};

/**
 * @class SalaPife
 * @brief Classe operacional que envelopa as regras do motor Pife e as amarra à rede do servidor.
 *
 * Estende o comportamento padrão de SalaBase, implementando as rotinas de gerenciamento
 * de conexões WebSocket síncronas, reparações de queda (reconexões) e controle de início de partidas.
 */
class SalaPife : public SalaBase {
private:
    Pife jogo_;                        ///< Instância da engine/motor lógico que dita as regras do Pife.
    std::vector<ConexaoPife> conexoes_; ///< Vetor contendo o par de conexões WebSocket ativas de cada assento.
    bool partidaIniciada_;             ///< Flag booleana indicando se o jogo já saiu do lobby de espera.

    /**
     * @brief Busca a struct interna de pareamento de rede baseada no ID do jogador.
     * @param idJogador ID do assento pesquisado.
     * @return Ponteiro para a struct ConexaoPife interna, ou nullptr se não localizado.
     */
    ConexaoPife* buscarConexaoDoJogador(int idJogador);

    /**
     * @brief Versão de sobrecarga constante (const) para busca da struct de pareamento de rede.
     * @param idJogador ID do assento pesquisado.
     * @return Ponteiro constante para a struct ConexaoPife interna, ou nullptr se não localizado.
     */
    const ConexaoPife* buscarConexaoDoJogador(int idJogador) const;

public:
    /**
     * @brief Construtor da classe SalaPife.
     * @param idSala Código identificador alfanumérico da sala.
     * @param maxJogadores Lotação máxima permitida de assentos.
     */
    SalaPife(const std::string& idSala, int maxJogadores);

    /**
     * @brief Insere um jogador novo na sala de Pife, registrando o seu nome e criando o soquete.
     * @param conexao Ponteiro para a conexão WebSocket aberta do Crow.
     * @param tokenReconexao Token validador gerado para futuras quedas do cliente.
     * @param nome Nome ou apelido escolhido pelo jogador.
     * @return O ID do assento atribuído ao jogador (0 a max-1), ou -1 em caso de erro/sala cheia.
     */
    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome);

    /**
     * @brief Processa o reatamento de um jogador que sofreu instabilidade de rede e apresentou um token válido.
     * @param conexao Nova conexão WebSocket aberta pelo navegador do cliente.
     * @param tokenReconexao Token que valida se a vaga preservada pertencia a este cliente.
     * @return O ID do assento recuperado, ou -1 se a validação falhar.
     */
    int reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);

    /**
     * @brief Remove o vínculo de uma conexão WebSocket da listagem de canais ativos.
     * @param conexao Ponteiro da conexão a ser removida.
     * @return true se o soquete foi encontrado e limpo, false caso contrário.
     */
    bool removerConexao(crow::websocket::connection* conexao);

    /**
     * @brief Descobre qual o ID do assento do jogador atrelado a uma conexão WebSocket.
     * @param conexao Ponteiro para a conexão do Crow.
     * @return ID numérico correspondente, ou -1 se a conexão não pertencer a esta sala.
     */
    int obterIdJogador(crow::websocket::connection* conexao) const;

    /**
     * @brief Retorna o ponteiro de rede do Crow associado a um ID de jogador.
     * @param idJogador ID do jogador consultado.
     * @return Ponteiro para o canal de WebSocket, ou nullptr se o jogador estiver offline/desconectado.
     */
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    /**
     * @brief Verifica se um determinado ponteiro de conexão WebSocket faz parte desta sala.
     * @param conexao Ponteiro para a conexão do Crow.
     * @return true se pertencer à sala, false caso contrário.
     */
    bool possuiConexao(crow::websocket::connection* conexao) const;

    /**
     * @brief Informa se o estado interno da partida atual de Pife já foi inicializado.
     * @return true se o jogo começou, false se está parado no lobby.
     */
    bool partidaIniciada() const;

    /**
     * @brief Altera a flag de inicialização para true, disparando o início oficial do jogo.
     */
    void iniciarPartida();

    /**
     * @brief Fornece acesso direto por referência para leitura/escrita no motor lógico interno do Pife.
     * @return Referência direta para a instância da classe Pife.
     */
    Pife& obterJogo();

    /**
     * @brief Fornece acesso apenas de leitura (constante) ao motor lógico interno do Pife.
     * @return Referência constante para a instância da classe Pife.
     */
    const Pife& obterJogo() const;

    /**
     * @brief Retorna uma referência constante para a coleção interna de conexões de rede da sala.
     * @return Referência constante para o vetor `std::vector<ConexaoPife>`.
     */
    const std::vector<ConexaoPife>& conexoes() const;
};

#endif