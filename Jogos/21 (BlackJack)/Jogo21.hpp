#pragma once
#include <vector>
#include "Baralho.hpp"
#include "Jogador21.hpp"

class Jogo21 {
private:
    Baralho* baralho_;                 ///< Ponteiro para o baralho da partida.
    std::vector<Jogador21*> jogadores_; ///< Lista de jogadores participantes (ponteiros).
    Jogador21* banca_;                 ///< A "Banca" (Dealer), que joga contra os jogadores.

public:
  
    Jogo21();

    ~Jogo21();

    void inicializarJogo(const std::vector<std::string>& nomesLista);
    void distribuirCartasIniciais();
    void turnoJogador(Jogador21* jogador);
    void turnoBanca();
    void determinarVencedores();
    void limparPartida();
    const std::vector<Jogador21*>& getJogadores() const;

    /** @return Retorna o ponteiro para o jogador da Banca. */
    Jogador21* getBanca() const;
};
