#ifndef JOGADOR_CONECTADO_HPP
#define JOGADOR_CONECTADO_HPP

#include <string>

//Verifica a situação do jogador

struct JogadorConectado {
    int idJogador;
    bool conectado;
    std::string tokenReconexao;
};

#endif