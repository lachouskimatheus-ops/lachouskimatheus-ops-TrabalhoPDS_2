/**
 * @file SalaPoker.hpp
 * @brief Definição da classe SalaPoker e da estrutura de conexões de rede associada.
 *
 * Acopla a lógica central do motor de Poker (MesaPoker) ao ecossistema multiplayer,
 * gerenciando as sessões dos jogadores locais e remotos.
 */

#pragma once

#include <string>
#include <vector>

#include "crow_all.h"
#include "json.hpp"

#include "coreAPI/SalaBase.hpp"
#include "MesaPoker.hpp"

using json = nlohmann::json;

/**
 * @struct ConexaoPoker
 * @brief Associa o ID do assento de um jogador ao seu respetivo ponteiro de WebSocket ativo no Crow.
 */
struct ConexaoPoker {
    int idJogador;                        ///< ID numérico identificador do assento alocado ao jogador.
    crow::websocket::connection* conexao; ///< Ponteiro para a conexão WebSocket correspondente do Crow.
};

/**
 * @class SalaPoker
 * @brief Extensão de SalaBase adaptada para coordenar partidas e conectividade de Poker.
 *
 * Integra e controla o motor lógico `MesaPoker`, fornecendo métodos de reconexão,
 * validação de integridade de fichas/trocas, início de turnos e serialização de estados em JSON.
 */
class SalaPoker : public SalaBase {
private:
    MesaPoker jogo_;                    ///< Instância da engine/motor que dita as regras e fases do Poker.
    std::vector<ConexaoPoker> conexoes_; ///< Coleção de conexões WebSocket ativas na sala.
    bool partidaIniciada_;              ///< Flag de controlo que sinaliza se o jogo já saiu do lobby.

    /**
     * @brief Procura internamente o par de rede atrelado ao ID do jogador solicitado.
     * @param idJogador ID do assento avaliado.
     * @return Ponteiro para a estrutura ConexaoPoker, ou nullptr se o jogador estiver offline.
     */
    ConexaoPoker* buscarConexaoDoJogador(int idJogador);

    /**
     * @brief Sobrecarga de leitura constante (const) para procurar o par de rede do jogador.
     * @param idJogador ID do assento avaliado.
     * @return Ponteiro constante para a estrutura ConexaoPoker, ou nullptr caso não exista.
     */
    const ConexaoPoker* buscarConexaoDoJogador(int idJogador) const;

    /**
     * @brief Traduz o enumerador de fase do jogo para uma string legível pelo JSON.
     * @fase Valor do enum FasePoker (ex: EscolhendoTrocas, Resultado).
     * @return String contendo o nome descritivo da fase.
     */
    static std::string faseParaString(FasePoker fase);

    /**
     * @brief Traduz o enumerador do modo de jogo para uma representação em string.
     * @modo Valor do enum ModoPoker (ex: ContraComputador, Multiplayer).
     * @return String contendo o nome descritivo do modo.
     */
    static std::string modoParaString(ModoPoker modo);

public:
    /**
     * @brief Construtor da classe SalaPoker.
     * @param idSala Código identificador alfanumérico gerado para a sala.
     * @param maxJogadores Lotação máxima configurada para a partida.
     * @param modo Regulamento da mesa (Multiplayer ou ContraComputador).
     */
    SalaPoker(const std::string& idSala, int maxJogadores, ModoPoker modo);

    /**
     * @brief Regista um jogador novo no lobby de Poker, associando a sua conexão e nome.
     * @param conexao Ponteiro para a conexão WebSocket do Crow.
     * @param tokenReconexao Chave de autenticação única atribuída para mitigar quedas de rede.
     * @param nome Nome de exibição ou apelido escolhido pelo jogador.
     * @return O ID do assento alocado (0 a max-1), ou -1 em caso de falha/sala cheia.
     */
    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome);

    /**
     * @brief Reabilita o canal de rede de um jogador desconectado através do seu token de validação.
     * @param conexao Novo ponteiro de WebSocket aberto pelo navegador.
     * @param tokenReconexao Token previamente gerado para comprovar a identidade do assento.
     * @return O ID do assento recuperado, ou -1 caso a validação falhe.
     */
    int reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);

    /**
     * @brief Remove o vínculo de uma conexão WebSocket específica do histórico de conexões da sala.
     * @param conexao Ponteiro para a ligação do Crow a ser descartada.
     * @return true se foi encontrada e removida, false caso contrário.
     */
    bool removerConexao(crow::websocket::connection* conexao);

    /**
     * @brief Descobre qual o ID do assento do jogador atrelado a uma determinada conexão.
     * @param conexao Ponteiro para a conexão do Crow.
     * @return ID numérico correspondente, ou -1 se a conexão não pertencer a esta sala.
     */
    int obterIdJogador(crow::websocket::connection* conexao) const;

    /**
     * @brief Retorna o ponteiro de rede do Crow associado a um ID de jogador.
     * @param idJogador ID do assento do jogador consultado.
     * @return Ponteiro para o canal de WebSocket, ou nullptr se o jogador estiver offline.
     */
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    /**
     * @brief Verifica se um determinado ponteiro de conexão WebSocket faz parte desta sala.
     * @param conexao Ponteiro para a conexão do Crow.
     * @return true se pertencer à sala, false caso contrário.
     */
    bool possuiConexao(crow::websocket::connection* conexao) const;

    /**
     * @brief Avalia se a sala ainda dispõe de assentos disponíveis para novos entrantes.
     * @return true se puder aceitar novos jogadores, false se estiver lotada.
     */
    bool podeReceberNovoJogador() const;

    /**
     * @brief Valida se o token apresentado cumpre os requisitos para reatar uma sessão instável.
     * @param tokenReconexao Cadeia de caracteres contendo o token.
     * @return true se pertencer a um utilizador desconectado da sala, false caso contrário.
     */
    bool podeReconectar(const std::string& tokenReconexao) const;

    /**
     * @brief Transiciona o estado do lobby para o início ativo do jogo, distribuindo as cartas.
     * @return true se a operação foi autorizada, false se as condições mínimas não foram atendidas.
     */
    bool iniciarPartida();

    /**
     * @brief Informa se o estado interno da partida atual de Poker já foi inicializado.
     * @return true se o jogo começou, false se está parado em espera no lobby.
     */
    bool partidaIniciada() const;

    /**
     * @brief Processa e valida os índices das cartas que um jogador escolheu trocar.
     * @param idJogador ID do assento do jogador.
     * @param indices Vetor com as posições (0 a 4) das cartas a serem substituídas.
     * @return true se a troca foi agendada/confirmada com sucesso, false caso contrário.
     */
    bool confirmarTroca(int idJogador, const std::vector<int>& indices);

    /**
     * @brief Comanda o motor lógico do jogo para avançar e iniciar um novo turno de Poker.
     * @return true se uma nova rodada pôde ser iniciada com sucesso.
     */
    bool iniciarNovaRodada();

    /**
     * @brief Serializa o cenário público ou privado atual da sala num formato JSON.
     * @param idJogador ID do jogador que solicita a atualização (usado para ocultar as mãos dos adversários).
     * @return Objeto nlohmann::json pronto para trafegar na rede.
     */
    json gerarJson(int idJogador) const;

    /**
     * @brief Retorna uma referência constante para o vetor interno de conexões de rede da sala.
     * @return Referência constante para o `std::vector<ConexaoPoker>`.
     */
    const std::vector<ConexaoPoker>& conexoes() const;
};