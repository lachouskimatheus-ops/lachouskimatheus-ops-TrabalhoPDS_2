/**
 * @file SalaTruco.hpp
 * @brief Definição da classe SalaTruco e do encapsulamento das conexões de rede do jogo.
 *
 * Integra a lógica central do Truco (Engine) e as regras de arbitragem (JuizTruco) ao
 * ecossistema multiplayer, gerenciando a distribuição por equipes e reconexões.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "crow_all.h"
#include "coreAPI/SalaBase.hpp"

#include "BaralhoTruco.hpp"
#include "Jogador_Truco.hpp"
#include "JuizTruco.hpp"
#include "Truco.hpp"

/**
 * @enum TipoTruco
 * @brief Define as variações de regras do Truco aceitas pelo sistema.
 */
enum class TipoTruco {
    Paulista, ///< Manilhas variam a cada rodada baseando-se no "Vira".
    Mineiro   ///< Manilhas fixas tradicionais (4 de Paus, 7 de Copas, Ás de Espadas, 7 de Ouros).
};

/**
 * @struct ConexaoTruco
 * @brief Estrutura de pareamento que vincula o assento de um jogador ao seu soquete WebSocket.
 */
struct ConexaoTruco {
    int idJogador;                        ///< ID identificador do assento alocado ao jogador na partida.
    crow::websocket::connection* conexao; ///< Ponteiro para a conexão WebSocket correspondente do Crow.
};

/**
 * @class SalaTruco
 * @brief Extensão de SalaBase especializada para mediar partidas de Truco Mineiro ou Paulista.
 *
 * Gerencia a composição das duas equipes, o ciclo de vida dos ponteiros dos jogadores locais,
 * a instância do baralho especializado, o juiz polimórfico adequado e o motor de regras (`Truco`).
 */
class SalaTruco : public SalaBase {
private:
    TipoTruco tipo_;                                            ///< Variante do regulamento selecionada para a sala.
    BaralhoTruco baralho_;                                      ///< Instância do baralho limpo de Truco (sem 4, 5, 6, 7 de espadas/paus/ouros conforme a regra).
    std::unique_ptr<JuizTruco> juiz_;                           ///< Ponteiro polimórfico para o calculador de força de cartas (JuizPaulistaTruco ou JuizMineiroTruco).
    std::unique_ptr<Truco> jogo_;                               ///< Instância da classe principal da engine/regras do Truco.
    std::vector<std::unique_ptr<Jogador_Truco>> jogadoresTruco_; ///< Vetor contendo a posse e os dados de jogo de cada participante.
    std::vector<ConexaoTruco> conexoes_;                         ///< Vetor de canais de comunicação ativos indexados por assento.
    bool partidaIniciada_;                                      ///< Flag de controlo que sinaliza se o jogo já saiu do lobby.

    /**
     * @brief Procura internamente a estrutura de rede conectada ao ID do assento solicitado.
     * @param idJogador ID do assento pesquisado.
     * @return Ponteiro para a estrutura ConexaoTruco, ou nullptr se o jogador estiver offline.
     */
    ConexaoTruco* buscarConexaoDoJogador(int idJogador);

    /**
     * @brief Sobrecarga constante (const) para busca da estrutura de rede do jogador.
     * @param idJogador ID do assento pesquisado.
     * @return Ponteiro constante para a estrutura ConexaoTruco, ou nullptr caso não exista.
     */
    const ConexaoTruco* buscarConexaoDoJogador(int idJogador) const;

public:
    /**
     * @brief Construtor da classe SalaTruco.
     * @param idSala Código identificador alfanumérico da sala.
     * @param tipo Regulamento escolhido para a mesa (Paulista ou Mineiro).
     * @param maxJogadores Lotação máxima configurada (tipicamente 2 ou 4 jogadores).
     */
    SalaTruco(const std::string& idSala, TipoTruco tipo, int maxJogadores);

    /**
     * @brief Aloca um jogador novo num assento vago, vinculando-o a uma equipe e canal de rede.
     * @param conexao Ponteiro para a conexão WebSocket do Crow.
     * @param tokenReconexao Token gerado pelo backend para autenticar futuras reconexões.
     * @param nome Nome de exibição do utilizador.
     * @param equipe Código da equipe pretendida (1 ou 2).
     * @return O ID do assento alocado (0 a max-1), ou -1 em caso de erro, equipe ou sala cheia.
     */
    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome, int equipe);

    /**
     * @brief Restabelece a conexão de um jogador que caiu recuperando o seu token identificador de assento.
     * @param conexao Novo ponteiro de WebSocket aberto.
     * @param tokenReconexao Chave que comprova a posse do assento reservado na sala.
     * @return O ID do assento reabilitado, ou -1 caso a validação falhe.
     */
    int reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);

    /**
     * @brief Remove o vínculo de uma conexão WebSocket do histórico de canais ativos da sala.
     * @param conexao Ponteiro para a ligação do Crow que se desconectou.
     * @return true se foi encontrada e removida com sucesso, false caso contrário.
     */
    bool removerConexao(crow::websocket::connection* conexao);

    /**
     * @brief Transiciona o lobby para o estado ativo de jogo, disparando a primeira distribuição de cartas.
     * @return true se a operação foi autorizada e inicializada com sucesso, false caso contrário.
     */
    bool iniciarPartida();

    /**
     * @brief Informa se o estado interno da partida atual de Truco já foi inicializado.
     * @return true se o jogo começou, false se está parado em espera no lobby.
     */
    bool partidaIniciada() const;

    /**
     * @brief Avalia se a sala possui capacidade para acomodar novos participantes.
     * @return true se puder receber novos jogadores, false se estiver cheia.
     */
    bool podeReceberNovoJogador() const;

    /**
     * @brief Valida se o token apresentado pertence a um utilizador legítimo que sofreu desconexão.
     * @param tokenReconexao Cadeia de caracteres contendo o token.
     * @return true se for passível de reconexão, false caso contrário.
     */
    bool podeReconectar(const std::string& tokenReconexao) const;

    /**
     * @brief Verifica se uma equipe específica ainda possui vagas abertas de assento.
     * @param equipe Código da equipe avaliada (1 ou 2).
     * @return true se houver vaga na equipe, false se estiver completa.
     */
    bool equipeDisponivel(int equipe) const;

    /**
     * @brief Descobre qual o ID do assento do jogador associado a uma determinada conexão.
     * @param conexao Ponteiro para a conexão do Crow.
     * @return ID numérico correspondente (0 a max-1), ou -1 se a conexão não fizer parte desta sala.
     */
    int obterIdJogador(crow::websocket::connection* conexao) const;

    /**
     * @brief Retorna o número total de jogadores atualmente alocados numa determinada equipe.
     * @param equipe Código da equipe consultada (1 ou 2).
     * @return Quantidade de jogadores presentes na equipe.
     */
    int quantidadeNaEquipe(int equipe) const;

    /**
     * @brief Retorna o ponteiro de rede do Crow associado a um ID de jogador.
     * @param idJogador ID do assento do jogador consultado.
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
     * @brief Retorna o regulamento ativo (Paulista ou Mineiro) configurado nesta sala.
     * @return O valor do enum TipoTruco correspondente.
     */
    TipoTruco tipo() const;

    /**
     * @brief Fornece acesso direto por referência para leitura/escrita no motor lógico interno do Truco.
     * @return Referência direta para a instância da classe Truco (`jogo_`).
     */
    Truco& obterJogo();

    /**
     * @brief Fornece acesso apenas de leitura (constante) ao motor lógico interno do Truco.
     * @return Referência constante para a instância da classe Truco (`jogo_`).
     */
    const Truco& obterJogo() const;

    /**
     * @brief Retorna uma referência constante para a coleção interna de conexões de rede da sala.
     * @return Referência constante para o vetor `std::vector<ConexaoTruco>`.
     */
    const std::vector<ConexaoTruco>& conexoes() const;
};