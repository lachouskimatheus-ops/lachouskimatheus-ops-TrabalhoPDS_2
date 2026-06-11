#pragma once

#include "Baralho.hpp"
#include "Jogador21.hpp"

#include <string>
#include <vector>

/**
 * @brief Fases possíveis de uma rodada de Blackjack/21.
 */
enum class Fase21 {
    NaoIniciado,
    EscolhendoAcoes,
    TurnoBanca,
    Resultado
};

/**
 * @brief Resultado individual de um jogador contra a banca.
 */
enum class Resultado21 {
    Indefinido,
    Vitoria,
    Derrota,
    Empate,
    Estourou
};

/**
 * @brief Controla uma partida de Blackjack/21.
 *
 * Esta classe contém apenas a lógica do jogo. Ela não usa entrada ou saída
 * de terminal, sendo adequada para integração com API/WebSocket.
 *
 * Fluxo:
 * 1. Inicializar jogadores.
 * 2. Distribuir duas cartas para cada jogador e para a banca.
 * 3. Cada jogador pode pedir carta ou parar.
 * 4. Quando todos param ou estouram, a banca compra até atingir pelo menos 17.
 * 5. O resultado é calculado individualmente entre cada jogador e a banca.
 */
class Jogo21 {
private:
    Baralho* baralho_;                    ///< Baralho da rodada.
    std::vector<Jogador21*> jogadores_;   ///< Jogadores humanos.
    Jogador21* banca_;                    ///< Banca/dealer.
    std::vector<Resultado21> resultados_; ///< Resultado de cada jogador.
    Fase21 fase_;                         ///< Fase atual da rodada.
    int rodada_;                          ///< Número da rodada atual.

    /**
     * @brief Busca o índice interno de um jogador pelo ID.
     * @param idJogador ID do jogador.
     * @return Índice no vetor jogadores_, ou -1 se não existir.
     */
    int indiceJogadorPorId(int idJogador) const;

    /**
     * @brief Compra uma carta do baralho para um jogador.
     * @param jogador Jogador que receberá a carta.
     * @return true se a carta foi comprada com sucesso.
     */
    bool comprarCartaPara(Jogador21* jogador);

    /**
     * @brief Verifica se todos os jogadores finalizaram suas ações.
     * @return true se todos pararam ou estouraram.
     */
    bool todosJogadoresFinalizaram();

    /**
     * @brief Reseta o baralho para uma nova rodada.
     */
    void prepararBaralho();

public:
    /**
     * @brief Construtor.
     */
    Jogo21();

    /**
     * @brief Destrutor.
     */
    ~Jogo21();

    /**
     * @brief Inicializa o jogo com os nomes dos jogadores.
     * @param nomesLista Lista de nomes dos jogadores humanos.
     * @return true se o jogo foi inicializado.
     */
    bool inicializarJogo(const std::vector<std::string>& nomesLista);

    /**
     * @brief Inicia uma nova rodada com os mesmos jogadores.
     * @return true se a rodada foi iniciada.
     */
    bool iniciarNovaRodada();

    /**
     * @brief Distribui duas cartas para cada jogador e para a banca.
     * @return true se a distribuição foi concluída.
     */
    bool distribuirCartasIniciais();

    /**
     * @brief Jogador pede uma carta.
     * @param idJogador ID do jogador.
     * @return true se a ação foi aceita.
     */
    bool pedirCarta(int idJogador);

    /**
     * @brief Jogador decide parar.
     * @param idJogador ID do jogador.
     * @return true se a ação foi aceita.
     */
    bool parar(int idJogador);

    /**
     * @brief Executa o turno da banca.
     *
     * A banca compra cartas enquanto sua pontuação for menor que 17.
     */
    void turnoBanca();

    /**
     * @brief Determina o resultado de todos os jogadores contra a banca.
     */
    void determinarVencedores();

    /**
     * @brief Limpa completamente a partida, removendo jogadores e cartas.
     */
    void limparPartida();

    /**
     * @brief Indica se o jogador pode agir no momento.
     * @param idJogador ID do jogador.
     * @return true se o jogador pode pedir carta ou parar.
     */
    bool podeAgir(int idJogador) const;

    /**
     * @brief Retorna os jogadores da partida.
     * @return Vetor de ponteiros para jogadores.
     */
    const std::vector<Jogador21*>& getJogadores() const;

    /**
     * @brief Retorna a banca.
     * @return Ponteiro para a banca.
     */
    Jogador21* getBanca() const;

    /**
     * @brief Retorna o resultado individual de um jogador.
     * @param idJogador ID do jogador.
     * @return Resultado do jogador.
     */
    Resultado21 resultadoJogador(int idJogador) const;

    /**
     * @brief Retorna todos os resultados.
     * @return Vetor de resultados por jogador.
     */
    const std::vector<Resultado21>& resultados() const;

    /**
     * @brief Retorna a fase atual.
     * @return Fase da rodada.
     */
    Fase21 fase() const;

    /**
     * @brief Retorna o número da rodada.
     * @return Rodada atual.
     */
    int rodada() const;

    /**
     * @brief Retorna a quantidade de jogadores humanos.
     * @return Número de jogadores.
     */
    int quantidadeJogadores() const;

    /**
     * @brief Retorna texto auxiliar para a fase.
     * @return String com nome da fase.
     */
    std::string nomeFase() const;

    /**
     * @brief Retorna texto auxiliar para o resultado.
     * @param resultado Resultado individual.
     * @return String com nome do resultado.
     */
    static std::string nomeResultado(Resultado21 resultado);
};
