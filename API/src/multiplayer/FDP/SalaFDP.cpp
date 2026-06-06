#include "multiplayer/FDP/SalaFDP.hpp"

#include "coreAPI/JsonConversor.hpp"

SalaFDP::SalaFDP()
    : mesa_(&baralho_, &placar_) {
}

void SalaFDP::conectarJogador(int idJogador) {
    jogadoresConectados_.insert(idJogador);
}

void SalaFDP::desconectarJogador(int idJogador) {
    jogadoresConectados_.erase(idJogador);
}

bool SalaFDP::jogarCarta(int idJogador, int indiceCarta) {

    if (mesa_.getJogadorDaVez() == nullptr) {
        return false;
    }

    if (mesa_.getJogadorDaVez()->getId() != idJogador) {
        return false;
    }

    return mesa_.jogarCarta(indiceCarta);
}

bool SalaFDP::apostar(int idJogador, int valor) {

    (void) idJogador;

    return mesa_.registrarAposta(valor);
}

nlohmann::json SalaFDP::gerarJson(int idJogador) {

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