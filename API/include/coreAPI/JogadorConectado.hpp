#ifndef JOGADOR_CONECTADO_HPP
#define JOGADOR_CONECTADO_HPP

#include <string>

struct JogadorConectado {
    int idJogador;
    bool conectado;
    std::string tokenReconexao;
};

#endif