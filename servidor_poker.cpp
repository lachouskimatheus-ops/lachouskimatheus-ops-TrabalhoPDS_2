#include "dependencias/crow_all.h"
#include "dependencias/json.hpp"

#include "Jogos/Poker/include/poker.h"
#include "Jogos/Core/include/Baralho.hpp"
#include "Jogos/Core/include/Carta.hpp"

#include <iostream>
#include <mutex>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

using json = nlohmann::json;

// ===============================
// VARIÁVEIS GLOBAIS DA MESA
// ===============================

Baralho baralhoMesa;

Poker jogadores[2];

std::string modoAtual = "COMPUTADOR";
std::string faseGlobal = "ESCOLHENDO_TROCAS";

int rodada = 1;

int pontosJogador1 = 0;
int pontosJogador2 = 0;
int empates = 0;

int ultimaTroca[2] = {0, 0};

bool confirmouTroca[2] = {false, false};

std::vector<int> indicesTroca[2];

crow::websocket::connection* conexaoJogador1 = nullptr;
crow::websocket::connection* conexaoJogador2 = nullptr;

std::map<crow::websocket::connection*, int> idPorConexao;
std::set<crow::websocket::connection*> conexoes;

std::mutex mtx;

// ===============================
// FUNÇÕES AUXILIARES DE CARTA
// ===============================

int valorNumericoServidor(Valor valor) {
    switch (valor) {
        case Valor::As: return 14;
        case Valor::Dois: return 2;
        case Valor::Tres: return 3;
        case Valor::Quatro: return 4;
        case Valor::Cinco: return 5;
        case Valor::Seis: return 6;
        case Valor::Sete: return 7;
        case Valor::Oito: return 8;
        case Valor::Nove: return 9;
        case Valor::Dez: return 10;
        case Valor::Valete: return 11;
        case Valor::Dama: return 12;
        case Valor::Rei: return 13;
        default: return 0;
    }
}

std::string simboloNaipe(Naipe naipe) {
    if (naipe == Naipe::Copa) return "♥";
    if (naipe == Naipe::Ouro) return "♦";
    if (naipe == Naipe::Espada) return "♠";
    if (naipe == Naipe::Paus) return "♣";
    return "?";
}

std::string corNaipe(Naipe naipe) {
    if (naipe == Naipe::Copa || naipe == Naipe::Ouro) {
        return "vermelha";
    }

    return "preta";
}

json cartaParaJson(const Carta& c) {
    return json{
        {"valor", (int)c.mostraValor()},
        {"naipe", (int)c.mostraNaipe()},
        {"valor_texto", c.valorString()},
        {"naipe_texto", c.naipeString()},
        {"simbolo", simboloNaipe(c.mostraNaipe())},
        {"cor", corNaipe(c.mostraNaipe())},
        {"texto", c.cartaString()},
        {"oculta", false}
    };
}

json cartaOcultaParaJson() {
    return json{
        {"valor", 0},
        {"naipe", 0},
        {"valor_texto", "?"},
        {"naipe_texto", "?"},
        {"simbolo", "🂠"},
        {"cor", "oculta"},
        {"texto", "Carta virada"},
        {"oculta", true}
    };
}

json maoParaJson(const Poker& jogador, bool revelar) {
    json maoJson = json::array();

    const std::vector<Carta>& mao = jogador.verMao();

    for (const auto& carta : mao) {
        if (revelar) {
            maoJson.push_back(cartaParaJson(carta));
        } else {
            maoJson.push_back(cartaOcultaParaJson());
        }
    }

    return maoJson;
}

// ===============================
// FUNÇÕES DE JOGO
// ===============================

void resetarPlacar() {
    rodada = 1;
    pontosJogador1 = 0;
    pontosJogador2 = 0;
    empates = 0;
}

void limparConfirmacoes() {
    confirmouTroca[0] = false;
    confirmouTroca[1] = false;

    indicesTroca[0].clear();
    indicesTroca[1].clear();

    ultimaTroca[0] = 0;
    ultimaTroca[1] = 0;
}

void distribuirCartas() {
    baralhoMesa.limpar();

    Baralho novoBaralho;
    novoBaralho.embaralhar();
    baralhoMesa = novoBaralho;

    jogadores[0].limparMao();
    jogadores[1].limparMao();

    for (int i = 0; i < 5; i++) {
        jogadores[0].receberCarta(baralhoMesa.retirarCarta());
        jogadores[1].receberCarta(baralhoMesa.retirarCarta());
    }

    limparConfirmacoes();
}

void novaRodadaComputador(bool incrementarRodada) {
    if (incrementarRodada) {
        rodada++;
    }

    distribuirCartas();

    modoAtual = "COMPUTADOR";
    faseGlobal = "ESCOLHENDO_TROCAS";
}

void novaRodadaMultiplayer(bool incrementarRodada) {
    if (incrementarRodada) {
        rodada++;
    }

    distribuirCartas();

    modoAtual = "MULTIPLAYER_2";
    faseGlobal = "ESCOLHENDO_TROCAS";
}

std::vector<int> escolherTrocasComputador(const Poker& computador) {
    std::vector<int> indices;

    int categoria = computador.avaliarMao();

    // Se já possui uma mão forte, mantém todas as cartas.
    if (categoria >= 4) {
        return indices;
    }

    const std::vector<Carta>& mao = computador.verMao();

    std::vector<int> frequencia(15, 0);

    for (const auto& carta : mao) {
        int valor = valorNumericoServidor(carta.mostraValor());

        if (valor >= 2 && valor <= 14) {
            frequencia[valor]++;
        }
    }

    // Mantém pares/trincas/quadras e troca cartas avulsas.
    for (int i = 0; i < (int)mao.size(); i++) {
        int valor = valorNumericoServidor(mao[i].mostraValor());

        if (frequencia[valor] == 1) {
            indices.push_back(i);
        }
    }

    // Se não tem nada, troca as três cartas mais baixas.
    if ((int)indices.size() == 5) {
        std::vector<std::pair<int, int>> valoresComIndice;

        for (int i = 0; i < (int)mao.size(); i++) {
            valoresComIndice.push_back({
                valorNumericoServidor(mao[i].mostraValor()),
                i
            });
        }

        std::sort(valoresComIndice.begin(), valoresComIndice.end());

        indices.clear();

        for (int i = 0; i < 3; i++) {
            indices.push_back(valoresComIndice[i].second);
        }
    }

    if (indices.size() > 3) {
        indices.resize(3);
    }

    return indices;
}

std::vector<int> lerIndicesTroca(const json& msg) {
    std::vector<int> indices;

    if (!msg.contains("indices") || !msg["indices"].is_array()) {
        return indices;
    }

    for (const auto& item : msg["indices"]) {
        if (item.is_number_integer()) {
            int valor = item.get<int>();

            if (valor >= 0 && valor < 5) {
                if (std::find(indices.begin(), indices.end(), valor) == indices.end()) {
                    indices.push_back(valor);
                }
            }
        }
    }

    if (indices.size() > 3) {
        indices.resize(3);
    }

    return indices;
}

// ===============================
// CONTROLE DE MULTIPLAYER
// ===============================

int obterIdJogador(crow::websocket::connection* conn) {
    if (idPorConexao.find(conn) != idPorConexao.end()) {
        return idPorConexao[conn];
    }

    return 0;
}

void resetarConexoesMultiplayer() {
    conexaoJogador1 = nullptr;
    conexaoJogador2 = nullptr;
    idPorConexao.clear();
}

int registrarJogadorMultiplayer(crow::websocket::connection* conn) {
    if (idPorConexao.find(conn) != idPorConexao.end()) {
        return idPorConexao[conn];
    }

    if (conexaoJogador1 == nullptr) {
        conexaoJogador1 = conn;
        idPorConexao[conn] = 1;
        return 1;
    }

    if (conexaoJogador2 == nullptr) {
        conexaoJogador2 = conn;
        idPorConexao[conn] = 2;
        return 2;
    }

    // 0 significa espectador/mesa cheia.
    idPorConexao[conn] = 0;
    return 0;
}

bool multiplayerCompleto() {
    return conexaoJogador1 != nullptr && conexaoJogador2 != nullptr;
}

void removerConexao(crow::websocket::connection* conn) {
    if (conexaoJogador1 == conn) {
        conexaoJogador1 = nullptr;
    }

    if (conexaoJogador2 == conn) {
        conexaoJogador2 = nullptr;
    }

    idPorConexao.erase(conn);

    if (modoAtual == "MULTIPLAYER_2") {
        faseGlobal = "AGUARDANDO_JOGADORES";
        jogadores[0].limparMao();
        jogadores[1].limparMao();
        limparConfirmacoes();
    }
}

// ===============================
// JSON PERSONALIZADO POR CLIENTE
// ===============================

json estadoParaCliente(crow::websocket::connection* conn) {
    json estado;

    int id = obterIdJogador(conn);

    int indiceEu = 0;
    int indiceAdversario = 1;

    if (modoAtual == "MULTIPLAYER_2" && id == 2) {
        indiceEu = 1;
        indiceAdversario = 0;
    }

    bool multiplayer = (modoAtual == "MULTIPLAYER_2");
    bool aguardandoJogadores = (faseGlobal == "AGUARDANDO_JOGADORES");
    bool resultadoRevelado = (faseGlobal == "RESULTADO");

    std::string faseCliente = faseGlobal;

    if (multiplayer &&
        faseGlobal == "ESCOLHENDO_TROCAS" &&
        id >= 1 &&
        confirmouTroca[indiceEu]) {
        faseCliente = "AGUARDANDO_OUTRO_JOGADOR";
    }

    estado["modo"] = modoAtual;
    estado["fase"] = faseCliente;
    estado["jogador_id"] = id;
    estado["rodada"] = rodada;

    estado["multiplayer"]["ativo"] = multiplayer;
    estado["multiplayer"]["jogadores_conectados"] =
        (conexaoJogador1 != nullptr ? 1 : 0) +
        (conexaoJogador2 != nullptr ? 1 : 0);
    estado["multiplayer"]["jogadores_necessarios"] =
        multiplayer ? 2 : 1;

    // Placar sempre é mostrado do ponto de vista do cliente.
    if (multiplayer && id == 2) {
        estado["placar"]["jogador"] = pontosJogador2;
        estado["placar"]["computador"] = pontosJogador1;
    } else {
        estado["placar"]["jogador"] = pontosJogador1;
        estado["placar"]["computador"] = pontosJogador2;
    }

    estado["placar"]["empates"] = empates;

    estado["trocas"]["jogador"] = ultimaTroca[indiceEu];
    estado["trocas"]["computador"] = ultimaTroca[indiceAdversario];

    estado["jogador"]["mao"] = maoParaJson(jogadores[indiceEu], true);
    estado["jogador"]["jogada"] = jogadores[indiceEu].nomeJogada();
    estado["jogador"]["forca"] = jogadores[indiceEu].avaliarMao();

    bool revelarAdversario = resultadoRevelado;

    estado["computador"]["mao"] = maoParaJson(jogadores[indiceAdversario], revelarAdversario);

    if (revelarAdversario) {
        estado["computador"]["jogada"] = jogadores[indiceAdversario].nomeJogada();
        estado["computador"]["forca"] = jogadores[indiceAdversario].avaliarMao();
    } else {
        estado["computador"]["jogada"] = "Oculta";
        estado["computador"]["forca"] = -1;
    }

    estado["resultado"] = 0;
    estado["vencedor"] = "Aguardando";

    if (modoAtual == "COMPUTADOR") {
        estado["titulo_jogador"] = "Jogador";
        estado["titulo_adversario"] = "Computador";
    } else {
        if (id == 1) {
            estado["titulo_jogador"] = "Jogador 1";
            estado["titulo_adversario"] = "Jogador 2";
        } else if (id == 2) {
            estado["titulo_jogador"] = "Jogador 2";
            estado["titulo_adversario"] = "Jogador 1";
        } else {
            estado["titulo_jogador"] = "Espectador";
            estado["titulo_adversario"] = "Mesa cheia";
        }
    }

    if (aguardandoJogadores) {
        estado["mensagem"] = "Aguardando outro jogador entrar na mesa.";
        estado["vencedor"] = "Aguardando jogadores";
        estado["jogador"]["jogada"] = "---";
        estado["computador"]["jogada"] = "---";
        return estado;
    }

    if (faseCliente == "ESCOLHENDO_TROCAS") {
        estado["mensagem"] = "Selecione até 3 cartas para trocar.";
        estado["vencedor"] = "Escolha suas cartas e confirme a troca.";
        return estado;
    }

    if (faseCliente == "AGUARDANDO_OUTRO_JOGADOR") {
        estado["mensagem"] = "Você confirmou sua troca.";
        estado["vencedor"] = "Aguardando o outro jogador confirmar.";
        return estado;
    }

    if (resultadoRevelado) {
        int resultadoGlobal = jogadores[0].compararCom(jogadores[1]);

        bool clienteVenceu = false;
        bool adversarioVenceu = false;

        if (modoAtual == "COMPUTADOR") {
            clienteVenceu = (resultadoGlobal == 1);
            adversarioVenceu = (resultadoGlobal == -1);
        } else if (id == 1) {
            clienteVenceu = (resultadoGlobal == 1);
            adversarioVenceu = (resultadoGlobal == -1);
        } else if (id == 2) {
            clienteVenceu = (resultadoGlobal == -1);
            adversarioVenceu = (resultadoGlobal == 1);
        }

        if (clienteVenceu) {
            estado["resultado"] = 1;
            estado["vencedor"] = "Jogador";
            estado["mensagem"] = "Você venceu a rodada!";
        } else if (adversarioVenceu) {
            estado["resultado"] = -1;
            estado["vencedor"] = "Computador";
            estado["mensagem"] = "O adversário venceu a rodada.";
        } else {
            estado["resultado"] = 0;
            estado["vencedor"] = "Empate";
            estado["mensagem"] = "A rodada terminou empatada.";
        }
    }

    return estado;
}

void enviarEstadoPara(crow::websocket::connection* conn) {
    if (conn != nullptr) {
        conn->send_text(estadoParaCliente(conn).dump());
    }
}

void broadcastEstados() {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto conn : conexoes) {
        enviarEstadoPara(conn);
    }
}

// ===============================
// AÇÕES DO SERVIDOR
// ===============================

void configurarModo(crow::websocket::connection* conn, const json& msg) {
    std::string modo = "computador";
    int quantidade = 1;

    if (msg.contains("modo") && msg["modo"].is_string()) {
        modo = msg["modo"];
    }

    if (msg.contains("quantidade_jogadores") && msg["quantidade_jogadores"].is_number_integer()) {
        quantidade = msg["quantidade_jogadores"].get<int>();
    }

    if (modo == "computador") {
        modoAtual = "COMPUTADOR";
        resetarConexoesMultiplayer();
        resetarPlacar();
        novaRodadaComputador(false);
        return;
    }

    if (modo == "multiplayer" && quantidade == 2) {
        if (modoAtual != "MULTIPLAYER_2") {
            modoAtual = "MULTIPLAYER_2";
            resetarConexoesMultiplayer();
            resetarPlacar();
            limparConfirmacoes();
            faseGlobal = "AGUARDANDO_JOGADORES";
        }

        registrarJogadorMultiplayer(conn);

        if (multiplayerCompleto()) {
            novaRodadaMultiplayer(false);
        }

        return;
    }

    // Qualquer outro modo ainda não implementado.
    modoAtual = "COMPUTADOR";
    resetarConexoesMultiplayer();
    resetarPlacar();
    novaRodadaComputador(false);
}

void processarTrocaComputador(const json& msg) {
    if (faseGlobal != "ESCOLHENDO_TROCAS") {
        return;
    }

    std::vector<int> indicesJogador = lerIndicesTroca(msg);

    jogadores[0].trocarCartas(indicesJogador, baralhoMesa);
    ultimaTroca[0] = (int)indicesJogador.size();

    std::vector<int> indicesComputador = escolherTrocasComputador(jogadores[1]);

    jogadores[1].trocarCartas(indicesComputador, baralhoMesa);
    ultimaTroca[1] = (int)indicesComputador.size();

    faseGlobal = "RESULTADO";

    int resultado = jogadores[0].compararCom(jogadores[1]);

    if (resultado == 1) {
        pontosJogador1++;
    } else if (resultado == -1) {
        pontosJogador2++;
    } else {
        empates++;
    }
}

void processarTrocaMultiplayer(crow::websocket::connection* conn, const json& msg) {
    if (faseGlobal != "ESCOLHENDO_TROCAS") {
        return;
    }

    int id = obterIdJogador(conn);

    if (id != 1 && id != 2) {
        return;
    }

    int indice = id - 1;

    if (confirmouTroca[indice]) {
        return;
    }

    indicesTroca[indice] = lerIndicesTroca(msg);
    ultimaTroca[indice] = (int)indicesTroca[indice].size();
    confirmouTroca[indice] = true;

    bool todosConfirmaram = confirmouTroca[0] && confirmouTroca[1];

    if (!todosConfirmaram) {
        return;
    }

    jogadores[0].trocarCartas(indicesTroca[0], baralhoMesa);
    jogadores[1].trocarCartas(indicesTroca[1], baralhoMesa);

    faseGlobal = "RESULTADO";

    int resultado = jogadores[0].compararCom(jogadores[1]);

    if (resultado == 1) {
        pontosJogador1++;
    } else if (resultado == -1) {
        pontosJogador2++;
    } else {
        empates++;
    }
}

void novaRodadaAtual() {
    if (modoAtual == "COMPUTADOR") {
        novaRodadaComputador(true);
        return;
    }

    if (modoAtual == "MULTIPLAYER_2") {
        if (multiplayerCompleto()) {
            novaRodadaMultiplayer(true);
        } else {
            faseGlobal = "AGUARDANDO_JOGADORES";
            jogadores[0].limparMao();
            jogadores[1].limparMao();
            limparConfirmacoes();
        }
    }
}

// ===============================
// MAIN
// ===============================

int main() {
    try {
        crow::SimpleApp app;

        std::cout << "Iniciando servidor do Poker..." << std::endl;

        novaRodadaComputador(false);

        CROW_WEBSOCKET_ROUTE(app, "/ws/poker")
            .onopen([&](crow::websocket::connection& conn) {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    conexoes.insert(&conn);
                }

                std::cout << "Cliente conectado ao Poker!" << std::endl;
                enviarEstadoPara(&conn);
            })
            .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    conexoes.erase(&conn);
                    removerConexao(&conn);
                }

                std::cout << "Cliente desconectado do Poker!" << std::endl;
                broadcastEstados();
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool) {
                try {
                    auto msg = json::parse(data);

                    if (!msg.contains("acao")) {
                        enviarEstadoPara(&conn);
                        return;
                    }

                    std::string acao = msg["acao"];

                    if (acao == "CONFIGURAR_MODO") {
                        configurarModo(&conn, msg);
                        broadcastEstados();
                        return;
                    }

                    if (acao == "OBTER_ESTADO_ATUAL") {
                        enviarEstadoPara(&conn);
                        return;
                    }

                    if (acao == "NOVA_RODADA" || acao == "NOVO_JOGO") {
                        novaRodadaAtual();
                        broadcastEstados();
                        return;
                    }

                    if (acao == "TROCAR_CARTAS") {
                        if (modoAtual == "COMPUTADOR") {
                            processarTrocaComputador(msg);
                        } else if (modoAtual == "MULTIPLAYER_2") {
                            processarTrocaMultiplayer(&conn, msg);
                        }

                        broadcastEstados();
                        return;
                    }

                    enviarEstadoPara(&conn);

                } catch (const std::exception& e) {
                    std::cerr << "Erro ao processar mensagem do Poker: " << e.what() << std::endl;
                }
            });

        CROW_ROUTE(app, "/")([]{
            crow::response res;
            res.set_static_file_info("frontend/pages/menu.html");
            return res;
        });

        CROW_ROUTE(app, "/pages/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/pages/" + path);
            return res;
        });

        CROW_ROUTE(app, "/assets/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/assets/" + path);
            return res;
        });

        CROW_ROUTE(app, "/css/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/css/" + path);
            return res;
        });

        CROW_ROUTE(app, "/js/<path>")([](const std::string& path){
            crow::response res;
            res.set_static_file_info("frontend/js/" + path);
            return res;
        });

        std::cout << "Servidor do Poker rodando em http://localhost:8080" << std::endl;
        std::cout << "WebSocket do Poker em ws://localhost:8080/ws/poker" << std::endl;

        app.port(8080).multithreaded().run();

    } catch (const std::exception& e) {
        std::cerr << "ERRO FATAL: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERRO FATAL DESCONHECIDO" << std::endl;
        return 1;
    }

    return 0;
}