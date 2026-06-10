/**
 * @file Truco.hpp
 * @brief Definição da classe Engine principal do Truco para servidores WebSocket e controle de estados.
 */

#pragma once

#include <string>
#include <vector>
#include "Baralho.hpp"
#include "Carta.hpp"
#include "Jogador_Truco.hpp"
#include "JuizTruco.hpp"

/**
 * @enum FaseTruco
 * @brief Estados discretos de ciclo de vida e turnos que guiam o motor de rede do Truco.
 */
enum class FaseTruco {
    AguardandoInicio,         ///< Mesa aberta aceitando conexões e divisão de equipes.
    AguardandoJogada,         ///< Aguardando que o jogador do turno selecione e descarte uma carta.
    AguardandoRespostaTruco,  ///< Fluxo travado aguardando a equipe oponente aceitar, recusar ou correr de um aumento de aposta.
    MaoFinalizada,            ///< Fim de uma rodada de 3 quedas. Contabilização e preparação para recolher as cartas.
    PartidaFinalizada         ///< Uma das equipes bateu o teto de 12 pontos. Jogo encerrado.
};

/**
 * @struct JogadaTruco
 * @brief Estrutura de acoplamento que registra qual carta foi colocada na mesa e por qual ID de jogador.
 */
struct JogadaTruco {
    int idJogador;     ///< ID identificador do assento do autor da jogada.
    Carta* carta;      ///< Ponteiro da Carta descartada na mesa.
};

/**
 * @class Truco
 * @brief Motor lógico e de estado completo para o jogo de Truco (Singleplayer ou Multiplayer Assíncrono).
 *
 * Esta classe é o coração da rota de API do Truco. Ela gerencia exaustivamente o estado interno de cada turno,
 * manipulação das manilhas, bloqueio de ações fora de hora, logs de eventos para transmissão JSON e as pontuações.
 */
class Truco {
private:
    std::vector<Jogador_Truco*> jogadores_;       ///< Vetor de ponteiros para as instâncias de jogadores conectados.
    std::vector<int> equipes_;                    ///< Mapeamento direto de ID do jogador para sua respectiva equipe (1 ou 2).
    std::vector<JogadaTruco> jogadasDaQueda_;     ///< Histórico ordenado de cartas lançadas na queda corrente.
    std::vector<Carta*> cartasDaMao_;              ///< Conjunto de cartas ativas postas em jogo na rodada completa.

    Carta* vira_;                                 ///< Ponteiro para a carta de Vira definida na rodada.
    Baralho* baralho_;                            ///< Ponteiro para o baralho limpo ativo na partida.
    JuizTruco* juiz_;                              ///< Ponteiro para a regra de arbitragem injetada (Mineiro ou Paulista).
    FaseTruco fase_;                              ///< A fase corrente em que as rotas da API devem processar as mensagens.

    int pontosEquipe1_;                           ///< Placar de pontos consolidados da Equipe 1.
    int pontosEquipe2_;                           ///< Placar de pontos consolidados da Equipe 2.
    int valorMao_;                                ///< Valor em pontos atual da rodada (1, 3, 6, 9 ou 12).
    int jogadorAtual_;                            ///< ID numérico do jogador que detém a vez de agir.
    int jogadorInicialMao_;                       ///< ID do jogador que cortou/iniciou a distribuição da mão.
    int jogadorInicialQueda_;                     ///< ID do jogador que detém a preferência de descarte na queda atual.
    int numeroQueda_;                             ///< Contador sequencial da queda da rodada (1ª, 2ª ou 3ª queda).
    int vitoriasEquipe1_;                         ///< Quantidade de quedas faturadas pela Equipe 1 na rodada corrente.
    int vitoriasEquipe2_;                         ///< Quantidade de quedas faturadas pela Equipe 2 na rodada corrente.
    int vencedorPrimeiraQueda_;                   ///< ID da equipe que ganhou a primeira queda (vital para regras de amarração).
    int vencedorUltimaQueda_;                     ///< ID da equipe vencedora da queda mais recente finalizada.
    int vencedorMao_;                             ///< ID da equipe declarada vencedora da mão corrente.
    int vencedorPartida_;                         ///< ID da equipe vencedora da partida (atingiu 12 pontos).
    int equipeQuePediu_;                          ///< ID da equipe que solicitou o aumento de aposta vigente (Truco/Seis/etc).
    int valorPedido_;                             ///< O valor proposto na aposta travada em negociação.

    bool maoTravada_;                             ///< Flag que congela ações normais durante janelas de decisão crítica de Truco.
    std::string nomePedidor_;                     ///< Guarda o nome de quem solicitou o Truco para exibição em painéis frontend.
    std::string ultimoEvento_;                    ///< Log rápido descritivo do último comando processado.
    std::string ultimaMensagem_;                  ///< Mensagem detalhada de status para transmissão para o WebSocket dos clientes.

    /**
     * @brief Valida se o ID fornecido condiz com as posições de assento permitidas.
     * @param idJogador ID do jogador teste.
     * @return true se o ID for válido, false caso contrário.
     */
    bool  jogadorValido(int idJogador) const;

    /**
     * @brief Valida se o código da equipe corresponde às equipes suportadas.
     * @param equipe Código da equipe (1 ou 2).
     * @return true se a equipe for válida.
     */
    bool  equipeValida(int equipe) const;

    /**
     * @brief Verifica se a mesa atingiu a cota máxima de 4 participantes divididos em duplas.
     * @return true se as equipes estão completas.
     */
    bool  equipesCompletas() const;

    /**
     * @brief Computa qual será o próximo valor incremental lógico da escala de Truco baseado no valor atual.
     * @param valorAtual Pontuação atual da mão.
     * @return Próximo degrau inteiro de pontos (ex: 1 -> 3, 3 -> 6, 6 -> 9, 9 -> 12).
     */
    int  proximoValorMao(int valorAtual) const;

    /**
     * @brief Executa o algoritmo de desempate e amarração para decidir qual equipe de fato leva os pontos da rodada.
     * @param vencedorQueda ID da equipe que venceu a última queda calculada.
     * @return ID da equipe vencedora final da mão, ou 0 se precisar continuar jogando.
     */
    int  resolverVencedorDaMao(int vencedorQueda) const;

    /**
     * @brief Limpa os históricos de cartas descartadas da mesa e zera os ponteiros locais.
     */
    void limparCartasDaMao();

    /**
     * @brief Puxa cartas do baralho limpo e reabastece a mão de todos os 4 jogadores inscritos.
     */
    void distribuirCartas();

    /**
     * @brief Desloca o anel de turnos mudando o jogadorAtual_ para a próxima posição à esquerda.
     */
    void avancarJogador();

    /**
     * @brief Coleta as cartas jogadas na queda, invoca o Juiz e calcula o encerramento da queda atual.
     */
    void concluirQueda();

    /**
     * @brief Prepara os estados de limpeza e define quem abre a rodada da queda seguinte.
     * @param jogadorInicial ID do jogador que ganhou o direito de começar.
     */
    void iniciarProximaQueda(int jogadorInicial);

    /**
     * @brief Consolida os pontos da rodada no placar fixo da equipe vencedora e checa condições de fim de jogo.
     * @param equipeVencedora ID da equipe (1 ou 2).
     * @param pontosGanhos Peso de pontos faturados na mão.
     */
    void finalizarMao(int equipeVencedora, int pontosGanhos);

public:
    /**
     * @brief Construtor principal da classe Engine do Truco.
     * @param juiz Instância injetada com as regras locais (Mineiro/Paulista).
     * @param baralho Instância injetada do baralho.
     */
    Truco(JuizTruco* juiz, Baralho* baralho);

    /**
     * @brief Destrutor. Libera os recursos alocados pelo gerenciamento do motor.
     */
    ~Truco();

    /**
     * @brief Inscreve um jogador associando-o a uma equipe específica.
     * @param jogador Ponteiro do jogador.
     * @param equipe Código da equipe escolhida (1 ou 2).
     * @return true se adicionado, false se a equipe estiver lotada ou jogo já iniciado.
     */
    bool adicionarJogador(Jogador_Truco* jogador, int equipe);

    /**
     * @brief Valida os requisitos mínimos e inicia a partida de Truco.
     * @return true se o início foi autorizado.
     */
    bool iniciarPartida();

    /**
     * @brief Limpa a mesa, redefine o baralho e inicia a distribuição de uma nova mão de pontos.
     * @return true se iniciada com sucesso.
     */
    bool iniciarNovaMao();

    /**
     * @brief Executa o comando de lançar uma carta da mão do jogador para o centro da mesa.
     * @param idJogador ID do solicitante.
     * @param indiceCarta Posição da carta na mão do jogador (0 a 2).
     * @return true se a jogada for válida e aceita pelas regras de turno.
     */
    bool jogarCarta(int idJogador, int indiceCarta);

    /**
     * @brief Registra o pedido de aumento de aposta (Truco ou subidas consecutivas).
     * @param idJogador ID do solicitante.
     * @return true se o pedido foi aceito para processamento e a fase alterada.
     */
    bool pedirTruco(int idJogador);

    /**
     * @brief Aceita o desafio de aumento de pontos proposto pela equipe adversária.
     * @param idJogador ID do jogador que respondeu aceitando.
     * @return true se processado e o jogo retomado ao ritmo normal.
     */
    bool aceitarTruco(int idJogador);

    /**
     * @brief Recusa o aumento de aposta proposto, correndo da mão e cedendo os pontos acumulados anteriores aos adversários.
     * @param idJogador ID do jogador que correu.
     * @return true se processado.
     */
    bool recusarTruco(int idJogador);

    /**
     * @brief Responde a um pedido de aumento subindo ainda mais o valor da aposta (ex: contra-proposta de Seis sobre Truco).
     * @param idJogador ID do autor do aumento.
     * @return true se a contra-proposta for legal e válida.
     */
    bool aumentarTruco(int idJogador);

    // =========================================================
    // Métodos de Consulta de Regras e Permissões (Getters Const)
    // =========================================================

    /** @brief Verifica se é a vez regulamentar do jogador descartar uma carta. */
    bool podeJogar(int idJogador) const;

    /** @brief Verifica se o jogador tem direito de solicitar Truco ou aumento neste momento do turno. */
    bool podePedirTruco(int idJogador) const;

    /** @brief Verifica se o jogador pertence à equipe elegível para responder ao Truco proposto. */
    bool podeResponderTruco(int idJogador) const;

    /** @brief Verifica se uma equipe ainda possui vagas abertas para assento de jogadores. */
    bool equipeDisponivel(int equipe, int maxJogadores) const;

    /** @brief Consulta a equipe vinculada a um determinado ID de jogador. */
    int getEquipeDoJogador(int idJogador) const;

    /** @brief Obtém a fase lógica/estado atual em que o jogo de Truco se encontra. */
    FaseTruco getFase() const;

    /** @brief Retorna o ID do jogador que detém o turno ativo. */
    int getJogadorAtual() const;

    /** @brief Retorna o número da queda corrente (1, 2 ou 3). */
    int getNumeroQueda() const;

    /** @brief Obtém a pontuação atual configurada que a mão vale. */
    int getValorMao() const;

    /** @brief Obtém a pontuação consolidada da Equipe 1 na partida. */
    int getPontosEquipe1() const;

    /** @brief Obtém a pontuação consolidada da Equipe 2 na partida. */
    int getPontosEquipe2() const;

    /** @brief Retorna o vencedor da última queda calculada. */
    int getVencedorUltimaQueda() const;

    /** @brief Retorna a equipe vencedora da mão de cartas atual. */
    int getVencedorMao() const;

    /** @brief Retorna a equipe vencedora absoluta da partida (bateu 12 pontos). */
    int getVencedorPartida() const;

    /** @brief Obtém o código da equipe que disparou a aposta pendente de resposta. */
    int getEquipeQuePediu() const;

    /** @brief Retorna o valor de pontos proposto na aposta pendente de resposta. */
    int getValorPedido() const;

    /** @brief Retorna o número de jogadores atualmente inscritos na mesa. */
    int getQuantidadeJogadores() const;

    /** @brief Retorna a quantidade de jogadores ativos alocados em uma equipe específica. */
    int getQuantidadeNaEquipe(int equipe) const;

    /** @brief Consulta o painel interno para saber qual será o próximo valor da escala de apostas se houver aumento. */
    int getProximoValorMao() const;

    /** @brief Retorna o ID do jogador eleito para abrir a queda atual. */
    int getJogadorInicialQueda() const;

    /** @brief Retorna a equipe que faturou a primeira queda da rodada. */
    int getVencedorPrimeiraQueda() const;

    /** @brief Retorna o número de quedas vencidas pela Equipe 1 na rodada atual. */
    int getVitoriasEquipe1() const;

    /** @brief Retorna o número de quedas vencidas pela Equipe 2 na rodada atual. */
    int getVitoriasEquipe2() const;

    /** @brief Verifica se a rodada está congelada aguardando resposta de aposta. */
    bool getMaoTravada() const;

    /** @brief Obtém o nome do jogador que efetuou a última solicitação de Truco. */
    const std::string& getNomePedidor() const;

    /** @brief Retorna o log de strings do último evento registrado pela engine. */
    const std::string& getUltimoEvento() const;

    /** @brief Retorna a string detalhada da última mensagem enviada aos WebSockets. */
    const std::string& getUltimaMensagem() const;

    /** @brief Retorna o ponteiro constante da carta exposta como Vira. */
    const Carta* getVira() const;

    /** @brief Retorna a referência constante para a coleção de cartas postas na queda atual. */
    const std::vector<JogadaTruco>& getJogadasDaQueda() const;

    /** @brief Retorna o vetor contendo a lista completa de ponteiros de jogadores da mesa. */
    const std::vector<Jogador_Truco*>& getJogadores() const;
};