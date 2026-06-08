#ifndef SALA_PIFE_HPP
#define SALA_PIFE_HPP

#include <string>
#include <vector>

#include <crow_all.h>

#include "coreAPI/SalaBase.hpp"
#include "Pife.hpp"

struct ConexaoPife {
    int idJogador;
    crow::websocket::connection* conexao;
};

class SalaPife : public SalaBase {
private:
    Pife jogo_;
    std::vector<ConexaoPife> conexoes_;
    bool partidaIniciada_;

public:
    SalaPife(const std::string& idSala, int maxJogadores);

    int adicionarJogador(crow::websocket::connection* conexao);

    bool removerConexao(crow::websocket::connection* conexao);

    int obterIdJogador(crow::websocket::connection* conexao) const;

    bool estaCheia() const;
    bool partidaIniciada() const;
    bool podeReceberJogador() const;

    void iniciarPartida();

    Pife& jogo();
    const Pife& jogo() const;

    std::vector<ConexaoPife>& conexoes();

    const std::vector<ConexaoPife>&
    conexoes() const;
};

#endif