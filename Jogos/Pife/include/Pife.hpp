#ifndef PIFE_HPP
#define PIFE_HPP

#include "Carta.hpp"
#include "Baralho.hpp"
#include "JogadorPife.hpp"
#include "RegrasPife.hpp"

#include <vector>


enum class FaseTurno {
    AguardandoCompra,
    AguardandoDescarte,
    Finalizado
};


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
    void distribuirCartas();
    bool jogadorValido(int idJogador) const;

    void reporBaralhoComDescarte();

public:
    Pife(int quantidadeJogadores);

    // Ações do jogo

    bool comprarBaralho(int idJogador);
    bool comprarMesa(int idJogador);
    bool colocarNaMesa(int idJogador, int indiceCarta);
    bool organizarMao(int idJogador);
    bool bati(int idJogador);
    void proximoJogador();

    // Verificação das ações permitidas

    bool podeComprarBaralho(int idJogador) const;
    bool podeComprarMesa(int idJogador) const;
    bool podeColocarNaMesa(int idJogador) const;
    bool podeBater(int idJogador) const;

    // Consulta do estado da partida

    bool jogoFinalizado() const;
    int consultarIndiceJogadorAtual() const;
    int consultarVencedor() const;
    FaseTurno consultarFaseTurno() const;
    int numeroDeJogadores() const;
    int quantidadeCartasBaralho() const;

    // Consulta dos jogadores

    JogadorPife& consultarJogador(int idJogador);
    const JogadorPife& consultarJogador(int idJogador) const;
    const std::vector<Carta>& consultarMao(int idJogador) const;

    // Consulta da mesa
    const std::vector<Carta>& consultarMesa() const;
    const Carta& consultarVira() const;

    bool cartaEhCoringa(const Carta& carta) const;

};

#endif