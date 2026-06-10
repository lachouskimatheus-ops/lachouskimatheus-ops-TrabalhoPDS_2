/**
 * @file Mesa.hpp
 * @brief Definição da estrutura de Mesa para partidas locais CLI de Truco.
 */

#pragma once
#include <vector>
#include "Jogador_Truco.hpp"
#include "Baralho.hpp"
#include "Juiz.hpp"  
#include "Carta.hpp"

/**
 * @class Mesa
 * @brief Abstração física de uma mesa de Truco em modo local textual.
 * * Coordena os assentos dos jogadores, o fluxo sequencial de quedas, a distribuição de cartas 
 * locais, o gerenciamento do placar de equipes até o teto de 12 pontos e o processamento de apostas de truco.
 */
class Mesa {
private:
    std::vector<Jogador_Truco*> jogadores; ///< Vetor de ponteiros para os jogadores da mesa.
    std::vector<Carta*> cartasNaMesa;      ///< Cartas descartadas e expostas na queda vigente.
    Carta* vira;                           ///< Ponteiro para a carta definida como Vira na mão.

    Baralho* baralho;                      ///< Ponteiro para a instância do baralho em uso.
    Juiz* juiz;                            ///< Ponteiro polimórfico para o motor de regras/juiz do jogo.

    int pontosEquipe1;                     ///< Placar geral acumulado de pontos da Equipe 1.
    int pontosEquipe2;                     ///< Placar geral acumulado de pontos da Equipe 2.

    int valorAtualMao;                     ///< Valor em pontos que a mão atual distribuída renderá (1, 3, 6, 9 ou 12).
    int nivelTruco;                        ///< Estado atual da escala de apostas (0=normal, 1=truco, 2=seis, 3=nove, 4=doze).

    /**
     * @brief Mapeia o índice do assento e retorna a qual equipe ele pertence.
     * @param indiceJogador ID numérico do jogador (0 a 3).
     * @return ID da equipe (1 para assentos 0 e 2, ou 2 para assentos 1 e 3).
     */
    int getEquipeDoJogador(int indiceJogador) const;

    /**
     * @brief Processa e gerencia a árvore de diálogo e decisão de uma solicitação de Truco enviada por um assento.
     * @param indiceJogadorPedindo ID numérico do jogador que disparou a aposta.
     * @return Código indicativo: equipe vencedora por recusa dos oponentes (1 ou 2), ou 0 se a aposta foi aceita.
     */
    int processarPedidoTruco(int indiceJogadorPedindo);

public:
    /**
     * @brief Construtor da Mesa de jogo local.
     * @param juizEscolhido Instância regulamentar de regras.
     * @param baralhoEscolhido Instância de baralho limpo filtrado.
     */
    Mesa(Juiz* juizEscolhido, Baralho* baralhoEscolhido);

    /**
     * @brief Destrutor da classe Mesa. Libera ponteiros alocados localmente na mesa.
     */
    ~Mesa();

    /**
     * @brief Vincula um objeto Jogador_Truco a um assento vago na mesa.
     * @param j Ponteiro do jogador.
     */
    void adicionarJogador(Jogador_Truco* j);

    /**
     * @brief Reseta as cartas anteriores, embaralha o maço e distribui 3 cartas para cada participante.
     */
    void prepararRodada();

    /**
     * @brief Gerencia e processa uma mão inteira composta por até 3 quedas alternadas.
     * @return ID da equipe que faturou os pontos da mão (1 ou 2), ou 0 em caso raro de empate absoluto.
     */
    int jogarTurno();

    /**
     * @brief Inicia o loop principal de jogo, executando turnos seguidos até que uma equipe alcance os 12 pontos regulamentares.
     */
    void jogarPartida();

    /**
     * @brief Consulta o juiz para analisar as cartas expostas na mesa e definir o vencedor da queda atual.
     * @return ID do assento vencedor ou código de empate.
     */
    int determinarVencedorDaQueda();

    /**
     * @brief Imprime no console a representação das cartas expostas na mesa e o Vira.
     */
    void exibirMesa() const;

    /**
     * @brief Imprime o placar geral comparativo de pontos entre a Equipe 1 e a Equipe 2.
     */
    void exibirPlacar() const;
};