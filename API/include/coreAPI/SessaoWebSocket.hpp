#ifndef SESSAO_WEBSOCKET_HPP
#define SESSAO_WEBSOCKET_HPP

#include <string>

struct SessaoWebSocket {
    std::string idSala;
    int idJogador;
    std::string tokenReconexao;
};

#endif