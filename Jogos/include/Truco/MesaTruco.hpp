/**
 * @file MesaTruco.hpp
 * @brief Definição da estrutura de mesa para partidas locais de Truco via terminal.
 */

#pragma once

#include <vector>

#include "Baralho.hpp"
#include "Carta.hpp"
#include "Jogador_Truco.hpp"
#include "Juiz.hpp"

/**
 * @class MesaTruco
 * @brief Abstração física de uma mesa de Truco em modo local textual.
 *
 * Coordena os assentos dos jogadores, o fluxo sequencial de quedas, a distribuição
 * de cartas, o gerenciamento do placar das equipes até 12 pontos e o processamento
 * dos pedidos de Truco e seus respectivos aumentos.
 */
class MesaTruco {
private:
    std::vector<Jogador_Truco*> jogadores; ///< Jogadores associados aos assentos da mesa.
    std::vector<Carta*> cartasNaMesa;      ///< Cartas expostas durante a queda atual.
    Carta* vira;                           ///< Carta utilizada como vira da mão atual.

    Baralho* baralho; ///< Baralho utilizado durante a partida.
    Juiz* juiz;       ///< Motor de regras responsável por comparar as cartas jogadas.

    int pontosEquipe1; ///< Pontuação acumulada da Equipe 1.
    int pontosEquipe2; ///< Pontuação acumulada da Equipe 2.

    int valorAtualMao; ///< Valor atual da mão: 1, 3, 6, 9 ou 12 pontos.
    int nivelTruco;    ///< Nível da aposta: 0 normal, 1 truco, 2 seis, 3 nove e 4 doze.

    /**
     * @brief Determina a equipe de um jogador a partir do índice do seu assento.
     * @param indiceJogador Índice do jogador na mesa.
     * @return 1 para jogadores dos assentos pares ou 2 para jogadores dos assentos ímpares.
     */
    int getEquipeDoJogador(int indiceJogador) const;

    /**
     * @brief Processa um pedido de Truco ou aumento feito por um jogador.
     *
     * A função solicita uma resposta da equipe adversária, permitindo aceitar,
     * recusar ou aumentar o valor da mão.
     *
     * @param indiceJogadorPedindo Índice do jogador que realizou o pedido.
     * @return Equipe vencedora caso o pedido seja recusado ou 0 caso o jogo continue.
     */
    int processarPedidoTruco(int indiceJogadorPedindo);

public:
    /**
     * @brief Constrói uma mesa local de Truco.
     * @param juizEscolhido Ponteiro para o juiz responsável pelas regras da partida.
     * @param baralhoEscolhido Ponteiro para o baralho utilizado na partida.
     */
    MesaTruco(Juiz* juizEscolhido, Baralho* baralhoEscolhido);

    /**
     * @brief Destrói a mesa local de Truco.
     *
     * A mesa não destrói o juiz, o baralho ou os jogadores, pois esses objetos
     * são recebidos externamente e não pertencem exclusivamente à mesa.
     */
    ~MesaTruco();

    /**
     * @brief Adiciona um jogador a um assento disponível da mesa.
     * @param jogador Ponteiro para o jogador que será adicionado.
     */
    void adicionarJogador(Jogador_Truco* jogador);

    /**
     * @brief Prepara uma nova mão da partida.
     *
     * Limpa as cartas da mesa, reinicia o valor da mão, embaralha o baralho,
     * define a vira e distribui três cartas para cada jogador.
     */
    void prepararRodada();

    /**
     * @brief Executa uma mão completa, composta por até três quedas.
     * @return Equipe vencedora da mão ou 0 caso nenhuma equipe pontue.
     */
    int jogarTurno();

    /**
     * @brief Executa a partida até que uma das equipes alcance 12 pontos.
     */
    void jogarPartida();

    /**
     * @brief Determina o vencedor da queda atual utilizando o juiz.
     * @return Índice do jogador vencedor ou o código de empate definido pelo juiz.
     */
    int determinarVencedorDaQueda();

    /**
     * @brief Exibe o estado atual da mesa, incluindo placar, valor da mão e vira.
     */
    void exibirMesa() const;

    /**
     * @brief Exibe o placar acumulado das duas equipes.
     */
    void exibirPlacar() const;
};