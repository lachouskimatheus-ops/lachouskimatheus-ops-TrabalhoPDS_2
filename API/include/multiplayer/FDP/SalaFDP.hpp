#pragma once

#include "MesaFDP.hpp"
#include "BaralhoSujo.hpp"
#include "Placar.hpp"

#include "json.hpp"

#include <unordered_set>

class SalaFDP {
private:

    BaralhoSujo baralho_;
    Placar placar_;
    MesaFDP mesa_;

    std::unordered_set<int> jogadoresConectados_;

public:

    SalaFDP();

    void conectarJogador(int idJogador);

    void desconectarJogador(int idJogador);

    bool jogarCarta(int idJogador, int indiceCarta);

    bool apostar(int idJogador, int valor);

    nlohmann::json gerarJson(int idJogador);

    bool vazaFinalizada();

    bool rodadaFinalizada();

    void finalizarVaza();

    void finalizarRodada();
};