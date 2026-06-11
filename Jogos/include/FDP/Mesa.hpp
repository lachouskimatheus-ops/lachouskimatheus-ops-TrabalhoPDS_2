#include <vector>
#include "BaralhoSujo.hpp"
#include "Jogador.hpp"

class Carta; ///< Declaração antecipada para evitar inclusão circular

/**
 * @brief Classe base que representa a mesa de um jogo de cartas.
 *
 * Gerencia o baralho e os jogadores participantes de uma partida.
 * Serve como base para mesas especializadas como MesaFDP.
 */
class Mesa {
protected:
    BaralhoSujo* baralho_;          ///< Ponteiro para o baralho utilizado na mesa
    std::vector<Jogador*> jogadores_; ///< Lista de jogadores sentados à mesa
    int indiceVez_;                 ///< Índice do jogador com a vez atual

public:
    /**
     * @brief Construtor.
     * @param baralho_mesa Ponteiro para o baralho que será usado na partida.
     */
    Mesa(BaralhoSujo* baralho_mesa);

    /** @brief Destrutor virtual. Garante destruição correta via polimorfismo. */
    virtual ~Mesa();

    /**
     * @brief Retorna a lista de jogadores na mesa.
     * @return Vetor com ponteiros para os jogadores.
     */
    std::vector<Jogador*> getJogadores() const { return jogadores_; }

    /**
     * @brief Adiciona um jogador à mesa.
     * @param j Ponteiro para o jogador a ser adicionado.
     */
    void adicionarJogador(Jogador* j);
};