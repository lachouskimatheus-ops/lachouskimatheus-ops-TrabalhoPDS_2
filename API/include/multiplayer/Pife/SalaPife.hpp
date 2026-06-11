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
 * @brief Classe operacional que integra o motor lógico do Pife à infraestrutura multiplayer.
 *
 * Estende SalaBase para gerenciar jogadores registrados, conexões WebSocket,
 * reconexões por token, início da partida e acesso ao estado interno do jogo.
 */
class SalaPife : public SalaBase {
private:
    Pife jogo_;                           ///< Instância do motor lógico responsável pelas regras do Pife.
    std::vector<ConexaoPife> conexoes_;   ///< Conexões WebSocket atualmente associadas aos jogadores da sala.
    bool partidaIniciada_;                ///< Indica se a partida já foi iniciada.

    /**
     * @brief Procura o registro de conexão associado a um jogador.
     * @param idJogador ID numérico do jogador pesquisado.
     * @return Ponteiro para o registro encontrado ou nullptr se não existir.
     */
    ConexaoPife* buscarConexaoDoJogador(int idJogador);

    /**
     * @brief Procura o registro de conexão associado a um jogador em contexto constante.
     * @param idJogador ID numérico do jogador pesquisado.
     * @return Ponteiro constante para o registro encontrado ou nullptr se não existir.
     */
    const ConexaoPife* buscarConexaoDoJogador(int idJogador) const;

public:
    /**
     * @brief Constrói uma sala multiplayer de Pife.
     * @param idSala Código identificador da sala.
     * @param maxJogadores Quantidade máxima de jogadores permitida.
     */
    SalaPife(const std::string& idSala, int maxJogadores);

    /**
     * @brief Adiciona um novo jogador ou reconecta um jogador já registrado.
     * @param conexao Ponteiro para a conexão WebSocket do jogador.
     * @param tokenReconexao Token utilizado para identificar e recuperar a sessão.
     * @param nome Nome ou apelido escolhido pelo jogador.
     * @return ID atribuído ao jogador ou -1 em caso de falha.
     */
    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome);

    /**
     * @brief Reconecta um jogador registrado anteriormente na sala.
     * @param conexao Ponteiro para a nova conexão WebSocket do jogador.
     * @param tokenReconexao Token utilizado para localizar o jogador registrado.
     * @return ID do jogador reconectado ou -1 se o token não for válido.
     */
    int reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);

    /**
     * @brief Remove uma conexão WebSocket ativa da sala.
     *
     * O jogador continua registrado na SalaBase, permitindo uma futura reconexão.
     *
     * @param conexao Ponteiro para a conexão que será removida.
     * @return true se a conexão foi encontrada e removida; false caso contrário.
     */
    bool removerConexao(crow::websocket::connection* conexao);

    /**
     * @brief Obtém o ID do jogador associado a uma conexão WebSocket.
     * @param conexao Ponteiro para a conexão pesquisada.
     * @return ID do jogador ou -1 se a conexão não estiver registrada.
     */
    int obterIdJogador(crow::websocket::connection* conexao) const;

    /**
     * @brief Obtém a conexão WebSocket associada a um jogador.
     * @param idJogador ID numérico do jogador.
     * @return Ponteiro para a conexão ou nullptr se o jogador estiver desconectado.
     */
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    /**
     * @brief Verifica se uma conexão WebSocket pertence à sala.
     * @param conexao Ponteiro para a conexão pesquisada.
     * @return true se a conexão estiver registrada; false caso contrário.
     */
    bool possuiConexao(crow::websocket::connection* conexao) const;

    /**
     * @brief Informa se a partida já foi iniciada.
     * @return true se a partida estiver em andamento ou finalizada; false se ainda estiver no lobby.
     */
    bool partidaIniciada() const;

    /**
     * @brief Verifica se a sala ainda pode receber um jogador novo.
     *
     * Um novo jogador somente pode entrar antes do início da partida e enquanto
     * ainda existir uma vaga disponível na sala.
     *
     * @return true se um novo jogador puder ser adicionado; false caso contrário.
     */
    bool podeReceberNovoJogador() const;

    /**
     * @brief Verifica se existe um jogador registrado com determinado token.
     * @param tokenReconexao Token de reconexão apresentado pelo cliente.
     * @return true se o token pertencer a um jogador registrado; false caso contrário.
     */
    bool podeReconectar(const std::string& tokenReconexao) const;

    /**
     * @brief Inicia a partida quando todos os jogadores esperados estiverem conectados.
     *
     * A função não realiza nenhuma alteração se a partida já tiver sido iniciada
     * ou se ainda houver jogadores ausentes.
     */
    void iniciarPartida();

    /**
     * @brief Fornece acesso de leitura e escrita ao motor lógico do Pife.
     * @return Referência para a instância interna de Pife.
     */
    Pife& jogo();

    /**
     * @brief Fornece acesso somente de leitura ao motor lógico do Pife.
     * @return Referência constante para a instância interna de Pife.
     */
    const Pife& jogo() const;

    /**
     * @brief Fornece acesso de leitura e escrita às conexões ativas da sala.
     * @return Referência para o vetor de registros de conexão.
     */
    std::vector<ConexaoPife>& conexoes();

    /**
     * @brief Fornece acesso somente de leitura às conexões ativas da sala.
     * @return Referência constante para o vetor de registros de conexão.
     */
    const std::vector<ConexaoPife>& conexoes() const;
};

#endif