#ifndef PIFE_HPP
#define PIFE_HPP

#include <vector>

#include "Baralho.hpp"
#include "Carta.hpp"
#include "ExcecoesPife.hpp"
#include "JogadorPife.hpp"
#include "RegrasPife.hpp"

/**
 * @enum FaseTurno
 * @brief Representa a fase atual do turno de uma partida de Pife.
 */
enum class FaseTurno {
    AguardandoCompra, 
    AguardandoDescarte, 
    Finalizado          
};

/**
 * @class Pife
 * @brief Controla o estado e o fluxo de uma partida de Pife.
 *
 * A classe gerencia o baralho, os jogadores, a pilha de descarte, a carta
 * vira, os turnos e a verificação de vitória.
 *
 * As funções que apenas consultam a possibilidade de realizar uma ação
 * retornam valores booleanos. As funções que efetivamente modificam o estado
 * da partida executam a ação ou lançam uma exceção derivada de ErroPife.
 */
class Pife {
private:
    Baralho baralho_;   
    std::vector<JogadorPife> jogadores_; 
    std::vector<Carta> mesa_;       
    Carta vira_;           
    int jogadorAtual_;     
    int vencedor_;                
    bool jogoFinalizado_;     
    FaseTurno faseTurno_;               

    /**
     * @brief Distribui as cartas iniciais e define a carta vira.
     *
     * Cada jogador recebe nove cartas. Em seguida, uma carta é retirada
     * para representar a vira da partida.
     *
     * @throw BaralhoIndisponivel Se não houver cartas suficientes.
     */
    void distribuirCartas();

    /**
     * @brief Verifica se um índice identifica um jogador existente.
     * @param idJogador Índice do jogador.
     * @return true se o índice for válido; false caso contrário.
     */
    bool jogadorValido(int idJogador) const;

    /**
     * @brief Valida a existência de um jogador.
     * @param idJogador Índice do jogador.
     * @throw JogadorInvalido Se o índice não identificar um jogador.
     */
    void validarJogador(int idJogador) const;

    /**
     * @brief Verifica se a partida ainda está em andamento.
     * @throw JogoFinalizado Se a partida já estiver encerrada.
     */
    void validarPartidaAtiva() const;

    /**
     * @brief Valida se um jogador pode agir no turno atual.
     * @param idJogador Índice do jogador.
     * @throw JogadorInvalido Se o jogador não existir.
     * @throw JogoFinalizado Se a partida já estiver encerrada.
     * @throw ForaDoTurno Se não for a vez do jogador.
     */
    void validarTurno(int idJogador) const;

    /**
     * @brief Reabastece o baralho utilizando as cartas descartadas.
     *
     * A carta mais recente da mesa permanece disponível para compra.
     * As demais cartas são transferidas para o baralho e embaralhadas.
     *
     * Caso a mesa não possua cartas suficientes, o baralho permanece vazio.
     */
    void reporBaralhoComDescarte();

public:
    /**
     * @brief Inicializa uma partida de Pife.
     * @param quantidadeJogadores Número de jogadores participantes.
     *
     * O construtor cria os jogadores, embaralha o baralho, distribui nove
     * cartas para cada participante e define a carta vira.
     *
     * @throw QuantidadeJogadoresInvalida Se a quantidade não estiver entre
     * dois e quatro jogadores.
     * @throw BaralhoIndisponivel Se não houver cartas suficientes.
     */
    explicit Pife(int quantidadeJogadores);

    /**
     * @name Ações da partida
     * @{
     */

    /**
     * @brief Compra uma carta do baralho.
     * @param idJogador Índice do jogador que realizará a compra.
     *
     * Caso o baralho esteja vazio, tenta reabastecê-lo utilizando as cartas
     * descartadas na mesa.
     *
     * Após a compra, a fase passa para FaseTurno::AguardandoDescarte.
     *
     * @throw JogadorInvalido Se o jogador não existir.
     * @throw JogoFinalizado Se a partida já estiver encerrada.
     * @throw ForaDoTurno Se não for a vez do jogador.
     * @throw FaseTurnoInvalida Se o jogador já tiver comprado.
     * @throw BaralhoIndisponivel Se nenhuma carta estiver disponível.
     */
    void comprarBaralho(int idJogador);

    /**
     * @brief Compra a carta mais recente da mesa.
     * @param idJogador Índice do jogador que realizará a compra.
     *
     * Após a compra, a fase passa para FaseTurno::AguardandoDescarte.
     *
     * @throw JogadorInvalido Se o jogador não existir.
     * @throw JogoFinalizado Se a partida já estiver encerrada.
     * @throw ForaDoTurno Se não for a vez do jogador.
     * @throw FaseTurnoInvalida Se o jogador já tiver comprado.
     * @throw MesaVazia Se não houver cartas descartadas.
     */
    void comprarMesa(int idJogador);

    /**
     * @brief Descarta uma carta da mão do jogador.
     * @param idJogador Índice do jogador que realizará o descarte.
     * @param indiceCarta Índice da carta dentro da mão.
     *
     * Após o descarte, a vez passa para o próximo jogador e a fase retorna
     * para FaseTurno::AguardandoCompra.
     *
     * @throw JogadorInvalido Se o jogador não existir.
     * @throw JogoFinalizado Se a partida já estiver encerrada.
     * @throw ForaDoTurno Se não for a vez do jogador.
     * @throw FaseTurnoInvalida Se o jogador ainda não tiver comprado.
     * @throw IndiceCartaInvalido Se o índice da carta for inválido.
     */
    void colocarNaMesa(int idJogador, int indiceCarta);

    /**
     * @brief Organiza a mão de um jogador.
     * @param idJogador Índice do jogador.
     *
     * A ordenação utilizada é definida por JogadorPife::organizarMao().
     *
     * @throw JogadorInvalido Se o jogador não existir.
     * @throw JogoFinalizado Se a partida já estiver encerrada.
     */
    void organizarMao(int idJogador);

    /**
     * @brief Tenta encerrar a partida declarando vitória.
     * @param idJogador Índice do jogador que deseja bater.
     *
     * O jogador pode bater antes da compra, com nove cartas, ou depois da
     * compra, com dez cartas. A mão deve formar combinações válidas conforme
     * as regras do Pife.
     *
     * @throw JogadorInvalido Se o jogador não existir.
     * @throw JogoFinalizado Se a partida já estiver encerrada.
     * @throw ForaDoTurno Se não for a vez do jogador.
     * @throw FaseTurnoInvalida Se não for possível bater naquele momento.
     * @throw MaoInvalida Se a mão não formar combinações vencedoras.
     */
    void bati(int idJogador);

    /**
     * @brief Passa a vez para o próximo jogador.
     *
     * Ao chegar ao último jogador, o índice retorna ao primeiro.
     */
    void proximoJogador();

    /** @} */

    /**
     * @name Verificação de ações permitidas
     * @{
     */

    /**
     * @brief Verifica se um jogador pode comprar do baralho.
     * @param idJogador Índice do jogador.
     * @return true se a compra for permitida; false caso contrário.
     */
    bool podeComprarBaralho(int idJogador) const;

    /**
     * @brief Verifica se um jogador pode comprar da mesa.
     * @param idJogador Índice do jogador.
     * @return true se a compra for permitida e a mesa não estiver vazia.
     */
    bool podeComprarMesa(int idJogador) const;

    /**
     * @brief Verifica se um jogador pode descartar.
     * @param idJogador Índice do jogador.
     * @return true se o descarte for permitido; false caso contrário.
     */
    bool podeColocarNaMesa(int idJogador) const;

    /**
     * @brief Verifica se um jogador pode tentar bater.
     * @param idJogador Índice do jogador.
     * @return true se o jogador puder declarar vitória naquele momento.
     *
     * Esta função verifica apenas o momento da ação e a quantidade de cartas.
     * Ela não garante que a mão possua combinações vencedoras.
     */
    bool podeBater(int idJogador) const;

    /** @} */

    /**
     * @name Consulta do estado da partida
     * @{
     */

    /**
     * @brief Informa se a partida terminou.
     * @return true se a partida estiver finalizada.
     */
    bool jogoFinalizado() const;

    /**
     * @brief Retorna o jogador com a vez atual.
     * @return Índice do jogador atual.
     */
    int consultarIndiceJogadorAtual() const;

    /**
     * @brief Retorna o vencedor da partida.
     * @return Índice do vencedor ou -1 se não houver vencedor.
     */
    int consultarVencedor() const;

    /**
     * @brief Retorna a fase atual do turno.
     * @return FaseTurno atual.
     */
    FaseTurno consultarFaseTurno() const;

    /**
     * @brief Retorna a quantidade de jogadores.
     * @return Número de jogadores participantes.
     */
    int numeroDeJogadores() const;

    /**
     * @brief Retorna a quantidade de cartas restantes no baralho.
     * @return Número de cartas disponíveis no baralho.
     */
    int quantidadeCartasBaralho() const;

    /**
     * @brief Verifica se uma carta representa o coringa da partida.
     * @param carta Carta que será verificada.
     * @return true se a carta for o coringa; false caso contrário.
     */
    bool cartaEhCoringa(const Carta& carta) const;

    /** @} */

    /**
     * @name Consulta dos jogadores
     * @{
     */

    /**
     * @brief Retorna um jogador por meio de seu índice.
     * @param idJogador Índice do jogador.
     * @return Referência para o jogador.
     * @throw JogadorInvalido Se o índice não identificar um jogador.
     */
    JogadorPife& consultarJogador(int idJogador);

    /**
     * @brief Retorna um jogador por meio de seu índice.
     * @param idJogador Índice do jogador.
     * @return Referência constante para o jogador.
     * @throw JogadorInvalido Se o índice não identificar um jogador.
     */
    const JogadorPife& consultarJogador(int idJogador) const;

    /**
     * @brief Retorna a mão de um jogador.
     * @param idJogador Índice do jogador.
     * @return Referência constante para o vetor de cartas.
     * @throw JogadorInvalido Se o índice não identificar um jogador.
     */
    const std::vector<Carta>& consultarMao(int idJogador) const;

    /** @} */

    /**
     * @name Consulta da mesa
     * @{
     */

    /**
     * @brief Retorna a pilha de descarte.
     * @return Referência constante para as cartas presentes na mesa.
     */
    const std::vector<Carta>& consultarMesa() const;

    /**
     * @brief Retorna a carta vira.
     * @return Referência constante para a carta vira.
     */
    const Carta& consultarVira() const;

    /** @} */
};

#endif