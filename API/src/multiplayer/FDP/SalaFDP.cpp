#include "multiplayer/FDP/SalaFDP.hpp"

#include "coreAPI/JsonConversor.hpp"
#include "JogadorFDP.hpp"

SalaFDP::SalaFDP()
    : mesa_(&baralho_, &placar_) {

    mesa_.adicionarJogador(new JogadorFDP(0, "Jogador 1"));
    mesa_.adicionarJogador(new JogadorFDP(1, "Jogador 2"));
    mesa_.adicionarJogador(new JogadorFDP(2, "Jogador 3"));
    mesa_.adicionarJogador(new JogadorFDP(3, "Jogador 4"));

    mesa_.iniciarPartida();
}

void SalaFDP::conectarJogador(int idJogador) {
    jogadoresConectados_.insert(idJogador);
}

void SalaFDP::desconectarJogador(int idJogador) {
    jogadoresConectados_.erase(idJogador);
}

bool SalaFDP::jogarCarta(int idJogador, int indiceCarta) {
    JogadorFDP* jogadorDaVez = mesa_.getJogadorDaVez();

    if (jogadorDaVez == nullptr) {
        return false;
    }

    if (jogadorDaVez->getId() != idJogador) {
        return false;
    }

    return mesa_.jogarCarta(indiceCarta);
}

bool SalaFDP::apostar(int idJogador, int valor) {
    JogadorFDP* jogadorDaVez = mesa_.getJogadorDaVez();

    if (jogadorDaVez == nullptr) {
        return false;
    }

    if (jogadorDaVez->getId() != idJogador) {
        return false;
    }

    if (!mesa_.registrarAposta(valor)) {
        return false;
    }

    if (mesa_.faseApostasFinalizada()) {
        mesa_.iniciarFaseDeCartas();
    }

    return true;
}

json SalaFDP::gerarJson(int idJogador) {
    return JsonConversor::mesaFdpParaJson(mesa_, idJogador);
}

bool SalaFDP::vazaFinalizada() {
    return mesa_.vazaFinalizada();
}

bool SalaFDP::rodadaFinalizada() {
    return mesa_.rodadaFinalizada();
}

void SalaFDP::finalizarVaza() {
    mesa_.apurarVencedorDaVaza();
}

void SalaFDP::finalizarRodada() {
    mesa_.finalizarRodada();
}