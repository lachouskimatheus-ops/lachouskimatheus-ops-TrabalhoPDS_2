#ifndef SALA_PIFE_HPP
#define SALA_PIFE_HPP

#include <string>
#include <vector>

#include "crow_all.h"

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

    ConexaoPife* buscarConexaoDoJogador(int idJogador);
    const ConexaoPife* buscarConexaoDoJogador(int idJogador) const;

public:
    SalaPife(const std::string& idSala, int maxJogadores);

    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome);
    int reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);

    bool removerConexao(crow::websocket::connection* conexao);

    int obterIdJogador(crow::websocket::connection* conexao) const;
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    bool possuiConexao(crow::websocket::connection* conexao) const;
    bool partidaIniciada() const;
    bool podeReceberNovoJogador() const;
    bool podeReconectar(const std::string& tokenReconexao) const;

    void iniciarPartida();

    Pife& jogo();
    const Pife& jogo() const;

    std::vector<ConexaoPife>& conexoes();
    const std::vector<ConexaoPife>& conexoes() const;
};

#endif