#pragma once

#include <memory>
#include <string>
#include <vector>

#include "crow_all.h"
#include "coreAPI/SalaBase.hpp"

#include "BaralhoTruco.hpp"
#include "Jogador_Truco.hpp"
#include "JuizTruco.hpp"
#include "Truco.hpp"

enum class TipoTruco {
    Paulista,
    Mineiro
};

struct ConexaoTruco {
    int idJogador;
    crow::websocket::connection* conexao;
};

class SalaTruco : public SalaBase {
private:
    TipoTruco tipo_;
    BaralhoTruco baralho_;
    std::unique_ptr<JuizTruco> juiz_;
    std::unique_ptr<Truco> jogo_;
    std::vector<std::unique_ptr<Jogador_Truco>> jogadoresTruco_;
    std::vector<ConexaoTruco> conexoes_;
    bool partidaIniciada_;

    ConexaoTruco* buscarConexaoDoJogador(int idJogador);
    const ConexaoTruco* buscarConexaoDoJogador(int idJogador) const;

public:
    SalaTruco(const std::string& idSala, TipoTruco tipo, int maxJogadores);

    int adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome, int equipe);
    int reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao);
    bool removerConexao(crow::websocket::connection* conexao);

    bool iniciarPartida();
    bool partidaIniciada() const;
    bool podeReceberNovoJogador() const;
    bool podeReconectar(const std::string& tokenReconexao) const;
    bool equipeDisponivel(int equipe) const;

    int obterIdJogador(crow::websocket::connection* conexao) const;
    int quantidadeNaEquipe(int equipe) const;

    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    TipoTruco tipo() const;

    Truco& jogo();
    const Truco& jogo() const;

    Jogador_Truco* jogadorTruco(int idJogador);
    const Jogador_Truco* jogadorTruco(int idJogador) const;

    std::vector<ConexaoTruco>& conexoes();
    const std::vector<ConexaoTruco>& conexoes() const;
};