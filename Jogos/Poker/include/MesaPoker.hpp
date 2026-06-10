/**
 * @file MesaPoker.hpp
 * @brief Definição da estrutura de controle de fluxo e estados de uma partida de Poker.
 */

#pragma once

#include <string>
#include <vector>
#include "Baralho.hpp"
#include "Poker.hpp"

/**
 * @enum FasePoker
 * @brief Representa os estados sequenciais permitidos durante as rodadas do jogo de Poker.
 */
enum class FasePoker {
    AguardandoJogadores, ///< Estado inicial onde a mesa está aguardando configurações ou conexões.
    EscolhendoTrocas,    ///< Janela de jogo onde os participantes avaliam suas mãos e decidem quais cartas descartar.
    Resultado            ///< Fase de Showdown onde as mãos são expostas, comparadas e os pontos atribuídos.
};

/**
 * @enum ModoPoker
 * @brief Define os tipos de oponentes configurados para a partida na mesa.
 */
enum class ModoPoker {
    ContraComputador, ///< Partida Singleplayer onde os demais assentos são controlados por Inteligência Artificial.
    Multiplayer       ///< Partida em rede controlada por múltiplos clientes via WebSockets.
};

/**
 * @class MesaPoker
 * @brief Classe de gerenciamento centralizador que dita as regras, turnos e estados da rodada de poker.
 *
 * Esta classe encapsula o baralho principal da mesa, os vetores de jogadores humanos/bots, 
 * pontuações acumuladas e as ações de validação para as fases de apostas e trocas de cartas.
 */
class MesaPoker {
private:
    Baralho baralho_;                            ///< O baralho de cartas utilizado na mesa.
    std::vector<Poker> jogadores_;               ///< Coleção contendo as mãos de Poker de cada assento da mesa.
    std::vector<std::string> nomes_;             ///< Vetor de strings com os nomes de exibição dos jogadores.
    std::vector<int> pontos_;                    ///< Placar acumulado de rodadas vencidas por cada jogador.
    std::vector<bool> confirmouTroca_;           ///< Flags que indicam se o jogador encerrou sua tomada de decisão na rodada atual.
    std::vector<std::vector<int>> trocasPendentes_; ///< Armazena temporariamente os índices das cartas que cada jogador marcou para descarte.
    std::vector<int> quantidadeUltimaTroca_;     ///< Registro de quantas cartas cada assento alterou no último turno de trocas.

    int quantidadeJogadores_; ///< Total de assentos configurados ativos na mesa.
    int rodada_;              ///< Contador interno da rodada atual do jogo.
    int vencedorRodada_;      ///< ID do índice do jogador que venceu a rodada vigente (-1 se indefinido).
    int empates_;             ///< Contador ou flag indicativa de cenários de empate em andamento.

    ModoPoker modo_;          ///< O modo atual configurado para a mesa (Singleplayer vs Multiplayer).
    FasePoker fase_;          ///< O estado lógico/fase atual em que a partida se encontra.

    /**
     * @brief Compra cartas do baralho e distribui uniformemente completando as mãos dos jogadores.
     */
    void distribuirCartas();

    /**
     * @brief Reseta o vetor de sinalizadores de confirmação de ações para um novo turno.
     */
    void limparConfirmacoes();

    /**
     * @brief Executa a tomada de decisão automatizada para os jogadores controlados por Inteligência Artificial.
     */
    void realizarJogadaComputador();

    /**
     * @brief Processa e efetiva os descartes e reabastecimentos de cartas acumulados em trocasPendentes_.
     */
    void executarTrocas();

    /**
     * @brief Realiza a comparação das mãos ao fim do turno, declara o vencedor e atualiza a pontuação.
     */
    void finalizarRodada();

    /**
     * @brief Algoritmo de IA básica que avalia a mão do computador e escolhe quais índices devem ser trocados.
     * @return Vetor contendo os índices recomendados para descarte.
     */
    std::vector<int> escolherTrocasComputador() const;

    /**
     * @brief Valida se os índices informados por um jogador para descarte estão dentro dos limites permitidos.
     * @param indices Vetor com as posições das cartas na mão.
     * @return true se todos os índices forem seguros e válidos, false caso contrário.
     */
    bool indicesTrocaValidos(const std::vector<int>& indices) const;

    /**
     * @brief Varre as flags de status e verifica se todos os jogadores aptos finalizaram suas escolhas.
     * @return true se todos confirmaram, permitindo o avanço de fase da mesa.
     */
    bool todosConfirmaram() const;

public:
    /**
     * @brief Construtor principal da Mesa de Poker.
     * @param quantidadeJogadores Número total de assentos participantes.
     * @param modo Enumeração sinalizando se haverá bots (ContraComputador) ou humanos (Multiplayer).
     */
    MesaPoker(int quantidadeJogadores, ModoPoker modo);

    /**
     * @brief Inicializa as estruturas de dados da mesa, nomes genéricos e prepara o ecossistema do jogo.
     */
    void iniciar();

    /**
     * @brief Prepara o baralho, limpa as mãos antigas e inicia um novo ciclo de rodada.
     * @return true se a nova rodada foi estabelecida com sucesso, false caso o baralho esteja esgotado.
     */
    bool iniciarNovaRodada();

    /**
     * @brief Associa um nome customizado a um ID de jogador na mesa.
     * @param idJogador Índice numérico do jogador (0 até quantidade-1).
     * @param nome String contendo o apelido a ser injetado.
     */
    void definirNomeJogador(int idJogador, const std::string& nome);

    /**
     * @brief Envia e agenda a solicitação de troca de cartas de um determinado jogador.
     * @param idJogador Índice numérico do solicitante.
     * @param indices Vetor com os índices das cartas a serem descartadas.
     * @return true se a requisição foi aceita e agendada, false se infringir regras ou for fora do turno.
     */
    bool confirmarTroca(int idJogador, const std::vector<int>& indices);

    /**
     * @brief Verifica se o jogador está em condições de registrar uma confirmação de troca.
     * @param idJogador Índice numérico do jogador.
     * @return true se ele for válido e ainda não tiver confirmado nesta fase.
     */
    bool podeConfirmarTroca(int idJogador) const;

    /**
     * @brief Valida se o ID fornecido está dentro do intervalo mapeado de assentos da mesa.
     * @param idJogador Índice numérico para teste.
     * @return true se o ID for válido, false se for menor que zero ou estourar o limite máximo.
     */
    bool jogadorValido(int idJogador) const;

    // =========================================================
    /// @name Getters e Consultas de Estado (Const)
    // =========================================================
    ///@{
    
    const Poker& jogador(int idJogador) const;
    const std::vector<int>& pontos() const;
    const std::vector<int>& quantidadeUltimaTroca() const;
    const std::vector<std::string>& nomes() const;
    int quantidadeJogadores() const;
    int rodada() const;
    int vencedorRodada() const;
    ModoPoker modo() const;
    FasePoker fase() const;
    
    ///@}
};