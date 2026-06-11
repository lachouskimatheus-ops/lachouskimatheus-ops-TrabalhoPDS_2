/**
 * @file SalaFDP.hpp
 * @brief Definição da classe SalaFDP e da estrutura de conexões WebSocket associada.
 *
 * Este ficheiro define as estruturas necessárias para acoplar a componente de rede
 * do Crow (WebSockets) com as regras e o motor lógico do jogo FDP.
 */

#pragma once

#include <string>
#include <vector>

#include "crow_all.h"
#include "json.hpp"

#include "coreAPI/SalaBase.hpp"

#include "BaralhoSujo.hpp"
#include "Placar.hpp"
#include "MesaFDP.hpp"

using json = nlohmann::json;

/**
 * @struct ConexaoFDP
 * @brief Associa um assento de jogador à sua respetiva ligação WebSocket ativa.
 */
struct ConexaoFDP {
    int idJogador;                        ///< ID numérico identificador do assento do jogador.
    crow::websocket::connection* conexao; ///< Ponteiro para a conexão WebSocket ativa do Crow.
};

/**
 * @class SalaFDP
 * @brief Especialização de SalaBase para gerir o estado de rede e a lógica de uma partida de FDP.
 *
 * Herda de SalaBase as operações base de jogadores e gere internamente os componentes do jogo
 * (baralho, placar e mesa), além do vetor de conexões WebSocket em tempo real.
 */
class SalaFDP : public SalaBase {
private:
    BaralhoSujo baralho_;             ///< Instância do baralho estendido utilizado no jogo.
    Placar placar_;                   ///< Componente responsável por gerir pontos e vidas dos jogadores.
    MesaFDP mesa_;                     ///< Instância do motor lógico que processa as regras da mesa.
    std::vector<ConexaoFDP> conexoes_; ///< Vetor contendo as conexões WebSocket ativas de cada jogador.

    bool partidaIniciada_;    ///< Flag que indica se o jogo já começou.
    bool processandoFimVaza_; ///< Flag de controlo para a transição e encerramento de uma vaza.

public:
    /**
     * @brief Construtor da SalaFDP.
     * @param idSala String identificadora que define o código da sala.
     * @param maxJogadores Limite de assentos configurados para a partida.
     */
    SalaFDP(const std::string& idSala, int maxJogadores);

    /**
     * @brief Obtém o ponteiro direto para o motor lógico da mesa de FDP.
     * @return Um ponteiro para o objeto MesaFDP interno.
     */
    MesaFDP* getMesa() {
        return &mesa;
    }

    /**
     * @brief Adiciona uma nova ligação WebSocket e regista o jogador na sala com o seu token.
     * @param conexao Ponteiro para a ligação WebSocket do Crow.
     * @param tokenReconexao Chave única gerada para validar a identidade do cliente.
     * @return O ID do assento atribuído ao jogador, ou -1 em caso de falha.
     */
    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);

    /**
     * @brief Remove uma ligação WebSocket específica do histórico de conexões da sala.
     * @param conexao Ponteiro para a ligação que deve ser removida.
     * @return true se a ligação foi localizada e removida, false caso contrário.
     */
    bool removerConexao(crow::websocket::connection* conexao);

    /**
     * @brief Procura qual é o ID do jogador dono de uma determinada ligação WebSocket.
     * @param conexao Ponteiro da ligação a ser avaliada.
     * @return O ID do assento do jogador, ou -1 se a conexão não estiver nesta sala.
     */
    int obterIdJogador(crow::websocket::connection* conexao) const;

    /**
     * @brief Obtém o ponteiro da ligação WebSocket ativa de um jogador através do seu ID.
     * @param idJogador ID do assento do jogador.
     * @return Ponteiro para a conexão do Crow, ou nullptr se o jogador estiver offline.
     */
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    /**
     * @brief Verifica se uma determinada ligação WebSocket pertence a esta sala.
     * @param conexao Ponteiro para a ligação do Crow.
     * @return true se a conexão estiver registada na sala, false caso contrário.
     */
    bool possuiConexao(crow::websocket::connection* conexao) const;

    /**
     * @brief Verifica se a sala ainda tem espaço para receber novos jogadores.
     * @return true se puder aceitar novos jogadores, false se estiver cheia.
     */
    bool podeReceberNovoJogador() const;

    /**
     * @brief Valida se um token de reconexão é elegível para reatar uma sessão instável.
     * @param tokenReconexao O token enviado pelo cliente.
     * @return true se o token pertencer a um jogador desconectado da sala, false caso contrário.
     */
    bool podeReconectar(const std::string& tokenReconexao) const;

    /**
     * @brief Verifica se a partida já foi iniciada na sala.
     * @return true se a partida começou, false se ainda está em espera.
     */
    bool partidaIniciada() const;

    /**
     * @brief Executa a ação de um jogador descartar uma carta na mesa.
     * @param idJogador ID do jogador que realiza a jogada.
     * @param indiceCarta Posição da carta no vetor da mão do jogador.
     * @return true se a jogada foi válida e processada, false caso contrário.
     */
    bool jogarCarta(int idJogador, int indiceCarta);

    /**
     * @brief Regista a aposta de truques/vazas feita por um jogador.
     * @param idJogador ID do jogador que está a apostar.
     * @param valor Quantidade de vazas que o jogador aposta que vai ganhar.
     * @return true se a aposta foi aceite pela lógica do jogo, false caso contrário.
     */
    bool apostar(int idJogador, int valor);

    /**
     * @brief Serializa o estado atual da sala num formato JSON seguro para o utilizador.
     * @param idJogador ID do jogador que está a pedir a atualização (usado para ocultar dados dos rivais).
     * @return Objeto nlohmann::json pronto para ser enviado via rede.
     */
    json gerarJson(int idJogador) const;

    /**
     * @brief Verifica se a vaza atual foi finalizada (todos os jogadores jogaram).
     * @return true se a vaza terminou, false caso contrário.
     */
    bool vazaFinalizada();

    /**
     * @brief Verifica se a rodada atual foi completamente encerrada.
     * @return true se a rodada terminou, false caso contrário.
     */
    bool rodadaFinalizada() const;

    /**
     * @brief Executa as rotinas de limpeza, pontuação e encerramento da vaza atual.
     */
    void finalizarVaza();

    /**
     * @brief Executa os procedimentos de pontuação, verificação de vidas e fecho da rodada.
     */
    void finalizarRodada();

    /**
     * @brief Consulta se a sala está no meio do processamento de fim de uma vaza.
     * @return true se estiver a processar, false caso contrário.
     */
    bool processandoFimVaza() const;

    /**
     * @brief Altera o estado da flag de processamento de fim de vaza.
     * @param processando O novo estado booleano para a flag.
     */
    void definirProcessandoFimVaza(bool processando);

    /**
     * @brief Retorna uma referência constante para o vetor de conexões WebSocket da sala.
     * @return Referência constante para o `std::vector<ConexaoFDP>`.
     */
    const std::vector<ConexaoFDP>& conexoes() const;
};