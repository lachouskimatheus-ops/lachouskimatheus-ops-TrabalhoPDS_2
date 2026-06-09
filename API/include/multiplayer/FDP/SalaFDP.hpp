#pragma once

#include <string>
#include <vector>

#include "crow_all.h"
#include "json.hpp"

#include "coreAPI/SalaBase.hpp"

#include "BaralhoSujo.hpp"
#include "Placar.hpp"
#include "MesaFDP.hpp"

using json = nlohmann::json;

struct ConexaoFDP {
    int idJogador;
    crow::websocket::connection* conexao;
};

class SalaFDP : public SalaBase {
private:
    BaralhoSujo baralho_;
    Placar placar_;
    MesaFDP mesa_;

    std::vector<ConexaoFDP> conexoes_;

    bool partidaIniciada_;
    bool processandoFimVaza_;

public:
    SalaFDP(
        const std::string& idSala,
        int maxJogadores
    );

    int adicionarJogador(
        crow::websocket::connection* conexao,
        const std::string& tokenReconexao
    );

    bool removerConexao(
        crow::websocket::connection* conexao
    );

    int obterIdJogador(
        crow::websocket::connection* conexao
    ) const;

    crow::websocket::connection*
    obterConexaoJogador(int idJogador) const;

    bool possuiConexao(
        crow::websocket::connection* conexao
    ) const;

    bool podeReceberNovoJogador() const;

    bool podeReconectar(
        const std::string& tokenReconexao
    ) const;

    bool partidaIniciada() const;

    bool jogarCarta(
        int idJogador,
        int indiceCarta
    );

    bool apostar(
        int idJogador,
        int valor
    );

    json gerarJson(int idJogador) const;

    bool vazaFinalizada();

    bool rodadaFinalizada() const;

    void finalizarVaza();

    void finalizarRodada();

    bool processandoFimVaza() const;

    void definirProcessandoFimVaza(
        bool processando
    );

    const std::vector<ConexaoFDP>&
    conexoes() const;
};