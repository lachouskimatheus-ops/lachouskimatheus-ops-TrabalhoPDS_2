#include "Poker.hpp"
#include "MesaPoker.hpp"
#include "Carta.hpp"

#include <cassert>
#include <iostream>
#include <vector>

Carta carta(Valor valor, Naipe naipe) {
    return Carta(valor, naipe);
}

Poker montarMao(const std::vector<Carta>& cartas) {
    Poker poker;

    for (const Carta& c : cartas) {
        bool recebeu = poker.receberCarta(c);
        assert(recebeu);
    }

    return poker;
}

void testarRankingBasico() {
    Poker royalFlush = montarMao({
        carta(Valor::Dez, Naipe::Copa),
        carta(Valor::Valete, Naipe::Copa),
        carta(Valor::Dama, Naipe::Copa),
        carta(Valor::Rei, Naipe::Copa),
        carta(Valor::As, Naipe::Copa)
    });

    assert(royalFlush.avaliarMao() == 9);
    assert(royalFlush.nomeDaJogada() == "Royal Flush");

    Poker quadra = montarMao({
        carta(Valor::Nove, Naipe::Copa),
        carta(Valor::Nove, Naipe::Ouro),
        carta(Valor::Nove, Naipe::Espada),
        carta(Valor::Nove, Naipe::Paus),
        carta(Valor::Dois, Naipe::Copa)
    });

    assert(quadra.avaliarMao() == 7);
    assert(quadra.nomeDaJogada() == "Quadra");

    Poker sequencia = montarMao({
        carta(Valor::Dez, Naipe::Ouro),
        carta(Valor::Valete, Naipe::Paus),
        carta(Valor::Dama, Naipe::Ouro),
        carta(Valor::Rei, Naipe::Paus),
        carta(Valor::As, Naipe::Copa)
    });

    assert(sequencia.avaliarMao() == 4);
    assert(sequencia.nomeDaJogada() == "Sequencia");

    Poker trinca = montarMao({
        carta(Valor::Seis, Naipe::Espada),
        carta(Valor::Seis, Naipe::Ouro),
        carta(Valor::Seis, Naipe::Paus),
        carta(Valor::Dois, Naipe::Copa),
        carta(Valor::Tres, Naipe::Copa)
    });

    assert(trinca.avaliarMao() == 3);
    assert(trinca.nomeDaJogada() == "Trinca");

    Poker par = montarMao({
        carta(Valor::Valete, Naipe::Ouro),
        carta(Valor::Valete, Naipe::Paus),
        carta(Valor::Tres, Naipe::Copa),
        carta(Valor::Dois, Naipe::Espada),
        carta(Valor::Rei, Naipe::Copa)
    });

    assert(par.avaliarMao() == 1);
    assert(par.nomeDaJogada() == "Um Par");

    Poker cartaAlta = montarMao({
        carta(Valor::As, Naipe::Ouro),
        carta(Valor::Dez, Naipe::Espada),
        carta(Valor::Sete, Naipe::Paus),
        carta(Valor::Cinco, Naipe::Ouro),
        carta(Valor::Dois, Naipe::Copa)
    });

    assert(cartaAlta.avaliarMao() == 0);
    assert(cartaAlta.nomeDaJogada() == "Carta Alta");

    std::cout << "[OK] Ranking basico das maos" << std::endl;
}

void testarComparacoes() {
    Poker sequencia = montarMao({
        carta(Valor::Dez, Naipe::Ouro),
        carta(Valor::Valete, Naipe::Paus),
        carta(Valor::Dama, Naipe::Ouro),
        carta(Valor::Rei, Naipe::Paus),
        carta(Valor::As, Naipe::Copa)
    });

    Poker trinca = montarMao({
        carta(Valor::Seis, Naipe::Espada),
        carta(Valor::Seis, Naipe::Ouro),
        carta(Valor::Seis, Naipe::Paus),
        carta(Valor::Dois, Naipe::Copa),
        carta(Valor::Tres, Naipe::Copa)
    });

    assert(sequencia.compararCom(trinca) == 1);
    assert(trinca.compararCom(sequencia) == -1);

    Poker par = montarMao({
        carta(Valor::Valete, Naipe::Ouro),
        carta(Valor::Valete, Naipe::Paus),
        carta(Valor::Tres, Naipe::Copa),
        carta(Valor::Dois, Naipe::Espada),
        carta(Valor::Rei, Naipe::Copa)
    });

    Poker cartaAlta = montarMao({
        carta(Valor::As, Naipe::Ouro),
        carta(Valor::Dez, Naipe::Espada),
        carta(Valor::Sete, Naipe::Paus),
        carta(Valor::Cinco, Naipe::Ouro),
        carta(Valor::Dois, Naipe::Copa)
    });

    assert(par.compararCom(cartaAlta) == 1);
    assert(cartaAlta.compararCom(par) == -1);

    std::cout << "[OK] Comparacoes" << std::endl;
}

void testarLimiteDeCartas() {
    Poker poker;

    assert(poker.receberCarta(carta(Valor::Dois, Naipe::Copa)));
    assert(poker.receberCarta(carta(Valor::Tres, Naipe::Copa)));
    assert(poker.receberCarta(carta(Valor::Quatro, Naipe::Copa)));
    assert(poker.receberCarta(carta(Valor::Cinco, Naipe::Copa)));
    assert(poker.receberCarta(carta(Valor::Seis, Naipe::Copa)));

    assert(!poker.receberCarta(carta(Valor::Sete, Naipe::Copa)));
    assert(poker.tamanhoMao() == 5);

    std::cout << "[OK] Limite de 5 cartas" << std::endl;
}

void testarMesaPoker() {
    MesaPoker mesa(2, ModoPoker::ContraComputador);

    mesa.definirNomeJogador(0, "Igor");
    mesa.iniciar();

    assert(mesa.quantidadeJogadores() == 2);
    assert(mesa.rodada() == 1);
    assert(mesa.fase() == FasePoker::EscolhendoTrocas);
    assert(mesa.jogador(0).tamanhoMao() == 5);
    assert(mesa.jogador(1).tamanhoMao() == 5);

    assert(mesa.confirmarTroca(0, {0, 1}));

    assert(mesa.fase() == FasePoker::Resultado);

    int totalPontos = mesa.pontos()[0] + mesa.pontos()[1] + mesa.empates();
    assert(totalPontos == 1);

    std::cout << "[OK] MesaPoker contra computador" << std::endl;
}

int main() {
    testarRankingBasico();
    testarComparacoes();
    testarLimiteDeCartas();
    testarMesaPoker();

    std::cout << "\nTODOS OS TESTES BASICOS DO POKER PASSARAM!" << std::endl;

    return 0;
}