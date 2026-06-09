#pragma once

#include <string>
#include <vector>

#include "crow_all.h"
#include "json.hpp"

#include "coreAPI/SalaBase.hpp"
#include "MesaPoker.hpp"

using json = nlohmann::json;

struct ConexaoPoker {
    int idJogador;
    crow::websocket::connection* conexao;
};

class SalaPoker : public SalaBase {
private:
    MesaPoker jogo_;
    std::vector<ConexaoPoker> conexoes_;
    bool partidaIniciada_;

    ConexaoPoker* buscarConexaoDoJogador(int idJogador);
    const ConexaoPoker* buscarConexaoDoJogador(int idJogador) const;

    static std::string faseParaString(FasePoker fase);
    static std::string modoParaString(ModoPoker modo);

public:
    SalaPoker(const std::string& idSala, int maxJogadores, ModoPoker modo);

    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome);
    int reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);
    bool removerConexao(crow::websocket::connection* conexao);

    int obterIdJogador(crow::websocket::connection* conexao) const;
    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    bool possuiConexao(crow::websocket::connection* conexao) const;
    bool podeReceberNovoJogador() const;
    bool podeReconectar(const std::string& tokenReconexao) const;

    bool iniciarPartida();
    bool partidaIniciada() const;

    bool confirmarTroca(int idJogador, const std::vector<int>& indices);
    bool iniciarNovaRodada();

    json gerarJson(int idJogadorSolicitante) const;

    MesaPoker& jogo();
    const MesaPoker& jogo() const;

    std::vector<ConexaoPoker>& conexoes();
    const std::vector<ConexaoPoker>& conexoes() const;
};