#pragma once

#include <string>
#include <vector>

#include "crow_all.h"
#include "json.hpp"

#include "coreAPI/SalaBase.hpp"
#include "BlackJack/Jogo21.hpp"

using json = nlohmann::json;

/**
 * @brief Associa um jogador da sala à sua conexão WebSocket.
 */
struct ConexaoBlackJack {
    int idJogador;
    crow::websocket::connection* conexao;
};

/**
 * @brief Gerencia uma sala de Blackjack/21 integrada à API.
 *
 * A classe controla:
 * - jogadores conectados;
 * - reconexão por token;
 * - início da partida;
 * - ações de pedir carta/parar;
 * - geração do estado JSON personalizado para cada jogador.
 */
class SalaBlackJack : public SalaBase {
private:
    Jogo21 jogo_;
    std::vector<ConexaoBlackJack> conexoes_;
    std::vector<std::string> nomes_;
    bool partidaIniciada_;

    ConexaoBlackJack* buscarConexaoDoJogador(int idJogador);
    const ConexaoBlackJack* buscarConexaoDoJogador(int idJogador) const;

    int idLogicoJogo(int idSalaBase) const;

    static std::string faseParaString(Fase21 fase);
    static std::string resultadoParaString(Resultado21 resultado);

    json cartaParaJson(const Carta* carta, bool oculta = false) const;
    json maoParaJson(const std::vector<Carta*>& mao, bool ocultarSegundaCarta) const;

public:
    SalaBlackJack(const std::string& idSala, int maxJogadores);

    int adicionarJogador(crow::websocket::connection* conexao,
                         const std::string& tokenReconexao,
                         const std::string& nome);

    int reconectarJogador(crow::websocket::connection* conexao,
                          const std::string& tokenReconexao);

    bool removerConexao(crow::websocket::connection* conexao);

    int obterIdJogador(crow::websocket::connection* conexao) const;

    crow::websocket::connection* obterConexaoJogador(int idJogador) const;

    bool possuiConexao(crow::websocket::connection* conexao) const;

    bool podeReceberNovoJogador() const;

    bool podeReconectar(const std::string& tokenReconexao) const;

    bool iniciarPartida();

    bool partidaIniciada() const;

    bool pedirCarta(int idJogador);

    bool parar(int idJogador);

    bool iniciarNovaRodada(int idSolicitante);

    json gerarJson(int idJogador) const;

    Jogo21& jogo();

    const Jogo21& jogo() const;

    std::vector<ConexaoBlackJack>& conexoes();

    const std::vector<ConexaoBlackJack>& conexoes() const;
};
