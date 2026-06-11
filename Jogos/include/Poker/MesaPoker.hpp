/**
 * @file MesaPoker.hpp
 * @brief Declara a classe responsável pelo controle de fluxo e pelos estados de uma partida de Poker.
 */

#pragma once

#include <string>
#include <vector>

#include "Baralho.hpp"
#include "Poker.hpp"

/**
 * @enum FasePoker
 * @brief Representa as fases possíveis de uma rodada de Poker.
 */
enum class FasePoker {
    AguardandoJogadores, ///< A mesa aguarda a inicialização e a configuração dos jogadores.
    EscolhendoTrocas,    ///< Os jogadores escolhem as cartas que desejam trocar.
    Resultado            ///< As mãos são comparadas e o resultado da rodada é apresentado.
};

/**
 * @enum ModoPoker
 * @brief Representa o modo de funcionamento da partida.
 */
enum class ModoPoker {
    ContraComputador, ///< Partida com um jogador humano e oponentes controlados pelo computador.
    Multiplayer       ///< Partida com jogadores humanos conectados à mesa.
};

/**
 * @class MesaPoker
 * @brief Gerencia os jogadores, o baralho, as rodadas e as fases de uma partida de Poker.
 *
 * A classe MesaPoker centraliza o fluxo da partida. Ela distribui as cartas,
 * recebe as escolhas de troca, executa as jogadas dos computadores, compara
 * as mãos e mantém o placar acumulado dos jogadores.
 */
class MesaPoker {
private:
    Baralho baralho_;                                  ///< Baralho utilizado durante a partida.
    std::vector<Poker> jogadores_;                     ///< Mãos e estados individuais dos jogadores.
    std::vector<std::string> nomes_;                   ///< Nomes associados aos jogadores.
    std::vector<int> pontos_;                          ///< Pontuação acumulada de cada jogador.
    std::vector<bool> confirmouTroca_;                 ///< Indica quais jogadores já confirmaram suas trocas.
    std::vector<std::vector<int>> trocasPendentes_;    ///< Índices das cartas selecionadas para troca.
    std::vector<int> quantidadeUltimaTroca_;           ///< Quantidade de cartas trocadas por cada jogador na última rodada.

    int quantidadeJogadores_; ///< Quantidade total de jogadores na mesa.
    int rodada_;              ///< Número da rodada atual.
    int vencedorRodada_;      ///< Identificador do vencedor da rodada ou -1 quando não há vencedor único.
    int empates_;             ///< Quantidade acumulada de rodadas empatadas.

    ModoPoker modo_; ///< Modo atual da partida.
    FasePoker fase_; ///< Fase atual da partida.

    /**
     * @brief Distribui cartas aos jogadores até completar suas mãos.
     */
    void distribuirCartas();

    /**
     * @brief Reinicia as confirmações de troca de todos os jogadores.
     */
    void limparConfirmacoes();

    /**
     * @brief Executa automaticamente as escolhas de troca dos jogadores controlados pelo computador.
     */
    void realizarJogadaComputador();

    /**
     * @brief Executa as trocas de cartas confirmadas pelos jogadores.
     *
     * As cartas selecionadas são removidas das mãos e substituídas por novas
     * cartas retiradas do baralho.
     */
    void executarTrocas();

    /**
     * @brief Compara as mãos, determina o resultado e atualiza o placar.
     */
    void finalizarRodada();

    /**
     * @brief Escolhe as cartas que o jogador controlado pelo computador deve trocar.
     *
     * @return Vetor contendo os índices das cartas selecionadas para troca.
     */
    std::vector<int> escolherTrocasComputador() const;

    /**
     * @brief Verifica se os índices informados para uma troca são válidos.
     *
     * @param indices Índices das cartas que serão trocadas.
     * @return true quando todos os índices são válidos; false caso contrário.
     */
    bool indicesTrocaValidos(const std::vector<int>& indices) const;

    /**
     * @brief Verifica se todos os jogadores já confirmaram suas escolhas de troca.
     *
     * @return true quando todos os jogadores confirmaram; false caso contrário.
     */
    bool todosConfirmaram() const;

public:
    /**
     * @brief Constrói uma mesa de Poker.
     *
     * @param quantidadeJogadores Quantidade de jogadores participantes.
     * @param modo Modo de funcionamento da partida.
     */
    MesaPoker(int quantidadeJogadores, ModoPoker modo);

    /**
     * @brief Inicializa os jogadores, os nomes e o estado inicial da mesa.
     */
    void iniciar();

    /**
     * @brief Inicia uma nova rodada.
     *
     * Limpa as mãos anteriores, prepara o baralho, distribui novas cartas e
     * altera a fase da mesa para a escolha de trocas.
     *
     * @return true quando a rodada foi iniciada com sucesso; false caso contrário.
     */
    bool iniciarNovaRodada();

    /**
     * @brief Define o nome de um jogador.
     *
     * @param idJogador Identificador do jogador.
     * @param nome Novo nome do jogador.
     */
    void definirNomeJogador(int idJogador, const std::string& nome);

    /**
     * @brief Registra e confirma as cartas que um jogador deseja trocar.
     *
     * @param idJogador Identificador do jogador.
     * @param indices Índices das cartas selecionadas para troca.
     * @return true quando a troca foi aceita; false quando a ação é inválida.
     */
    bool confirmarTroca(int idJogador, const std::vector<int>& indices);

    /**
     * @brief Verifica se um jogador pode confirmar sua troca.
     *
     * @param idJogador Identificador do jogador.
     * @return true quando o jogador pode confirmar; false caso contrário.
     */
    bool podeConfirmarTroca(int idJogador) const;

    /**
     * @brief Verifica se um identificador corresponde a um jogador existente.
     *
     * @param idJogador Identificador que será validado.
     * @return true quando o identificador é válido; false caso contrário.
     */
    bool jogadorValido(int idJogador) const;

    /**
     * @name Consultas de estado
     * @{
     */

    /**
     * @brief Retorna os dados de um jogador.
     *
     * @param idJogador Identificador do jogador.
     * @return Referência constante para o objeto Poker do jogador.
     */
    const Poker& jogador(int idJogador) const;

    /**
     * @brief Retorna o nome de um jogador.
     *
     * @param idJogador Identificador do jogador.
     * @return Referência constante para o nome do jogador.
     */
    const std::string& nomeJogador(int idJogador) const;

    /**
     * @brief Retorna o placar dos jogadores.
     *
     * @return Referência constante para o vetor de pontuações.
     */
    const std::vector<int>& pontos() const;

    /**
     * @brief Retorna a quantidade de cartas trocadas por cada jogador na última rodada.
     *
     * @return Referência constante para o vetor de quantidades.
     */
    const std::vector<int>& quantidadeUltimaTroca() const;

    /**
     * @brief Retorna os nomes de todos os jogadores.
     *
     * @return Referência constante para o vetor de nomes.
     */
    const std::vector<std::string>& nomes() const;

    /**
     * @brief Verifica se um jogador já confirmou sua troca na rodada atual.
     *
     * @param idJogador Identificador do jogador.
     * @return true quando o jogador já confirmou; false caso contrário.
     */
    bool jogadorConfirmouTroca(int idJogador) const;

    /**
     * @brief Retorna a quantidade de jogadores da mesa.
     *
     * @return Quantidade de jogadores.
     */
    int quantidadeJogadores() const;

    /**
     * @brief Retorna o número da rodada atual.
     *
     * @return Número da rodada.
     */
    int rodada() const;

    /**
     * @brief Retorna o identificador do vencedor da última rodada.
     *
     * @return Identificador do vencedor ou -1 quando não existe vencedor único.
     */
    int vencedorRodada() const;

    /**
     * @brief Retorna a quantidade acumulada de empates.
     *
     * @return Quantidade de rodadas empatadas.
     */
    int empates() const;

    /**
     * @brief Retorna o modo atual da partida.
     *
     * @return Modo de funcionamento da mesa.
     */
    ModoPoker modo() const;

    /**
     * @brief Retorna a fase atual da partida.
     *
     * @return Fase atual da mesa.
     */
    FasePoker fase() const;

    /** @} */
};