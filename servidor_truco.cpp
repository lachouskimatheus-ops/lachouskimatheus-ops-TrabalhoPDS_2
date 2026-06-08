#include "dependencias/crow_all.h"
#include "dependencias/json.hpp"
#include "Jogos/Truco/include/Mesa.hpp"
#include "Jogos/Truco/include/BaralhoSujo.hpp"
#include "Jogos/Truco/include/JuizPaulista.hpp"
#include "Jogos/Truco/include/JuizMineiro.hpp"
#include "Jogos/Truco/include/Jogador_Truco.hpp"
 
#include <mutex>
#include <set>
#include <vector>
#include <string>
 
using json = nlohmann::json;
 
// ==========================================
// ESTADO DA PARTIDA
// ==========================================
 
struct JogadorConectado {
    int id;
    std::string nome;
    int equipe; // 1 ou 2
    std::vector<Carta*> mao;
    crow::websocket::connection* conn;
};
 
struct EstadoTruco {
    std::vector<JogadorConectado> jogadores;
 
    // Jogo
    BaralhoSujo* baralho = nullptr;
    Juiz* juiz = nullptr;
    Carta* vira = nullptr;
    std::vector<Carta*> cartasNaMesa;
 
    // Pontuação
    int pontosEq1 = 0;
    int pontosEq2 = 0;
 
    // Estado da mão
    int quedaAtual = 1;
    int vitoriasEq1 = 0;
    int vitoriasEq2 = 0;
    int vencedorPrimeira = 0;
    int valorMao = 1;
    int nivelTruco = 0; // 0=normal, 1=truco, 2=seis, 3=nove, 4=doze
 
    // Truco
    bool aguardandoRespostaTruco = false;
    int equipeRespondendo = 0;
    int equipePedindo = 0;
 
    // Vez
    int jogadorDaVez = 0; // índice no vetor jogadores
    int inicioRodada = 0;
 
    bool partidaEncerrada = false;
    std::string ultimoEvento = "";
    int vencedorQueda = 0;
    int vencedorMao = 0;
    int equipeVencedora = 0;
};
 
// ==========================================
// HELPERS
// ==========================================
 
std::string nomesNivel[] = {"", "Truco", "Seis", "Nove", "Doze"};
int valoresNivel[]       = {1, 3, 6, 9, 12};
 
json cartaParaJson(const Carta* c) {
    std::string naipeStr;
    switch (c->getNaipe()) {
        case Naipe::paus:    naipeStr = "paus"; break;
        case Naipe::copas:   naipeStr = "copas"; break;
        case Naipe::espadas: naipeStr = "espadas"; break;
        default:             naipeStr = "ouros"; break;
    }
    return json{ {"valor", c->getValor()}, {"naipe", naipeStr} };
}
 
int getEquipe(int indiceJogador) {
    return (indiceJogador % 2 == 0) ? 1 : 2;
}
 
json construirEstado(const EstadoTruco& estado) {
    json j;
 
    j["pontos_equipe1"] = estado.pontosEq1;
    j["pontos_equipe2"] = estado.pontosEq2;
    j["valor_mao"]      = estado.valorMao;
    j["queda_atual"]    = estado.quedaAtual;
    j["nivel_truco"]    = estado.nivelTruco;
    j["aguardando_resposta_truco"] = estado.aguardandoRespostaTruco;
    j["equipe_respondendo"] = estado.equipeRespondendo;
    j["nome_nivel_truco"]   = nomesNivel[estado.nivelTruco < 4 ? estado.nivelTruco + 1 : 4];
    j["valor_se_aceito"]    = valoresNivel[estado.nivelTruco < 4 ? estado.nivelTruco + 1 : 4];
 
    // Vira
    if (estado.vira) j["vira"] = cartaParaJson(estado.vira);
 
    // Cartas na mesa
    json mesaArr = json::array();
    for (auto c : estado.cartasNaMesa) mesaArr.push_back(cartaParaJson(c));
    j["cartas_na_mesa"] = mesaArr;
 
    // Jogador da vez (id)
    if (!estado.jogadores.empty())
        j["jogador_da_vez"] = estado.jogadores[estado.jogadorDaVez].id;
 
    // Jogadores
    json jogadoresArr = json::array();
    for (int i = 0; i < (int)estado.jogadores.size(); i++) {
        const auto& jog = estado.jogadores[i];
        json jj;
        jj["id"]     = jog.id;
        jj["nome"]   = jog.nome;
        jj["equipe"] = jog.equipe;
        jj["cartas_na_mao"] = (int)jog.mao.size();
 
        // Manda as cartas do próprio jogador
        json maoArr = json::array();
        for (auto c : jog.mao) maoArr.push_back(cartaParaJson(c));
        jj["mao"] = maoArr;
 
        jogadoresArr.push_back(jj);
    }
    j["jogadores"] = jogadoresArr;
 
    // Evento
    j["evento"] = estado.ultimoEvento;
    j["vencedor_queda"] = estado.vencedorQueda;
    j["vencedor_mao"]   = estado.vencedorMao;
    j["equipe_vencedora"] = estado.equipeVencedora;
    j["pontos_ganhos"]    = estado.valorMao;
 
    return j;
}
 
void broadcast(EstadoTruco& estado, const std::string& msg) {
    for (auto& j : estado.jogadores) {
        if (j.conn) j.conn->send_text(msg);
    }
}
 
// ==========================================
// LÓGICA DO JOGO
// ==========================================
 
void prepararRodada(EstadoTruco& estado) {
    estado.cartasNaMesa.clear();
    estado.valorMao   = 1;
    estado.nivelTruco = 0;
    estado.quedaAtual = 1;
    estado.vitoriasEq1 = 0;
    estado.vitoriasEq2 = 0;
    estado.vencedorPrimeira = 0;
    estado.aguardandoRespostaTruco = false;
 
    estado.baralho->inicializar();
    estado.baralho->embaralhar();
 
    estado.vira = estado.baralho->puxarCarta();
 
    for (auto& jog : estado.jogadores) {
        jog.mao.clear();
        for (int i = 0; i < 3; i++) jog.mao.push_back(estado.baralho->puxarCarta());
    }
 
    estado.jogadorDaVez = estado.inicioRodada;
}
 
// Retorna equipe vencedora (1 ou 2), ou 0 empate
int resolverQueda(EstadoTruco& estado) {
    std::vector<Carta*> cartas = estado.cartasNaMesa;
    int idx = estado.juiz->decidirVencedor(cartas, *estado.vira, false);
    if (idx < 0) return 0;
    return getEquipe(idx);
}
 
void processarFimQueda(EstadoTruco& estado) {
    int equipeVenc = resolverQueda(estado);
    estado.vencedorQueda = equipeVenc;
    estado.ultimoEvento = "FIM_QUEDA";
 
    if (estado.quedaAtual == 1) estado.vencedorPrimeira = equipeVenc;
 
    if (equipeVenc == 1) estado.vitoriasEq1++;
    else if (equipeVenc == 2) estado.vitoriasEq2++;
 
    // Checa vitória da mão
    bool fimMao = false;
    int vencedorMao = 0;
 
    if (estado.vitoriasEq1 == 2) { fimMao = true; vencedorMao = 1; }
    else if (estado.vitoriasEq2 == 2) { fimMao = true; vencedorMao = 2; }
    else if (estado.quedaAtual == 2 && estado.vencedorPrimeira == 0 && equipeVenc != 0) {
        fimMao = true; vencedorMao = equipeVenc;
    }
    else if (estado.quedaAtual == 2 && estado.vencedorPrimeira != 0 && equipeVenc == 0) {
        fimMao = true; vencedorMao = estado.vencedorPrimeira;
    }
    else if (estado.quedaAtual == 3) {
        fimMao = true;
        if (equipeVenc != 0) vencedorMao = equipeVenc;
        else if (estado.vencedorPrimeira != 0) vencedorMao = estado.vencedorPrimeira;
        else vencedorMao = 0;
    }
 
    if (fimMao) {
        estado.vencedorMao = vencedorMao;
        estado.ultimoEvento = "FIM_MAO";
 
        if (vencedorMao == 1) estado.pontosEq1 += estado.valorMao;
        else if (vencedorMao == 2) estado.pontosEq2 += estado.valorMao;
 
        if (estado.pontosEq1 >= 12 || estado.pontosEq2 >= 12) {
            estado.equipeVencedora = (estado.pontosEq1 >= 12) ? 1 : 2;
            estado.ultimoEvento = "FIM_PARTIDA";
            estado.partidaEncerrada = true;
        } else {
            // Prepara próxima mão
            estado.inicioRodada = (estado.inicioRodada + 1) % (int)estado.jogadores.size();
            prepararRodada(estado);
        }
    } else {
        // Próxima queda
        estado.quedaAtual++;
        estado.cartasNaMesa.clear();
        // Quem venceu a queda começa a próxima
        if (equipeVenc != 0) {
            for (int i = 0; i < (int)estado.jogadores.size(); i++) {
                if (getEquipe(i) == equipeVenc) { estado.jogadorDaVez = i; break; }
            }
        }
    }
}
 
// ==========================================
// MAIN
// ==========================================
int main() {
    try {
        crow::SimpleApp app;
 
        EstadoTruco estado;
        estado.baralho = new BaralhoSujo();
        estado.juiz    = new JuizPaulista(); // padrão; podemos receber por parâmetro depois
 
        std::mutex mtx;
        std::set<crow::websocket::connection*> conexoes;
 
        CROW_WEBSOCKET_ROUTE(app, "/ws/truco")
            .onopen([&](crow::websocket::connection& conn) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.insert(&conn);
                // Manda estado atual para o recém-conectado
                conn.send_text(construirEstado(estado).dump());
            })
            .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.erase(&conn);
                for (auto& j : estado.jogadores) {
                    if (j.conn == &conn) j.conn = nullptr;
                }
            })
            .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool) {
                std::lock_guard<std::mutex> lock(mtx);
                try {
                    auto msg = json::parse(data);
                    std::string acao = msg["acao"];
                    int jogadorId = msg.value("jogador_id", -1);
 
                    estado.ultimoEvento = "";
                    estado.vencedorQueda = 0;
                    estado.vencedorMao = 0;
                    estado.equipeVencedora = 0;
 
                    // --- ENTRAR ---
                    if (acao == "ENTRAR") {
                        std::string nome = msg.value("nome", "Jogador " + std::to_string(jogadorId));
                        JogadorConectado jog;
                        jog.id     = jogadorId;
                        jog.nome   = nome;
                        jog.equipe = getEquipe((int)estado.jogadores.size());
                        jog.conn   = &conn;
                        estado.jogadores.push_back(jog);
 
                        // Inicia quando tiver 2 ou 4 jogadores
                        int n = (int)estado.jogadores.size();
                        if (n == 2 || n == 4) {
                            prepararRodada(estado);
                        }
                    }
 
                    // --- JOGAR CARTA ---
                    else if (acao == "JOGAR_CARTA") {
                        // Acha o jogador
                        int idxJogador = -1;
                        for (int i = 0; i < (int)estado.jogadores.size(); i++) {
                            if (estado.jogadores[i].id == jogadorId) { idxJogador = i; break; }
                        }
 
                        if (idxJogador != estado.jogadorDaVez) return; // Não é sua vez
                        if (estado.aguardandoRespostaTruco) return;     // Tem truco pendente
 
                        int indice = msg["indice"];
                        auto& mao = estado.jogadores[idxJogador].mao;
                        if (indice < 0 || indice >= (int)mao.size()) return;
 
                        Carta* cartaJogada = mao[indice];
                        mao.erase(mao.begin() + indice);
                        estado.cartasNaMesa.push_back(cartaJogada);
 
                        // Avança a vez
                        estado.jogadorDaVez = (estado.jogadorDaVez + 1) % (int)estado.jogadores.size();
 
                        // Todos jogaram nessa queda?
                        if ((int)estado.cartasNaMesa.size() == (int)estado.jogadores.size()) {
                            processarFimQueda(estado);
                        }
                    }
 
                    // --- PEDIR TRUCO ---
                    else if (acao == "PEDIR_TRUCO") {
                        if (estado.aguardandoRespostaTruco) return;
                        if (estado.nivelTruco >= 4) return;
 
                        int idxJogador = -1;
                        for (int i = 0; i < (int)estado.jogadores.size(); i++) {
                            if (estado.jogadores[i].id == jogadorId) { idxJogador = i; break; }
                        }
                        if (idxJogador != estado.jogadorDaVez) return;
 
                        estado.nivelTruco++;
                        estado.equipePedindo   = getEquipe(idxJogador);
                        estado.equipeRespondendo = (estado.equipePedindo == 1) ? 2 : 1;
                        estado.aguardandoRespostaTruco = true;
                        estado.ultimoEvento = "TRUCO_PEDIDO";
 
                        // Quem pediu entra no nome
                        json extra;
                        extra["nome_pedidor"] = estado.jogadores[idxJogador].nome;
                    }
 
                    // --- RESPONDER TRUCO ---
                    else if (acao == "RESPONDER_TRUCO") {
                        if (!estado.aguardandoRespostaTruco) return;
 
                        int idxJogador = -1;
                        for (int i = 0; i < (int)estado.jogadores.size(); i++) {
                            if (estado.jogadores[i].id == jogadorId) { idxJogador = i; break; }
                        }
                        if (getEquipe(idxJogador) != estado.equipeRespondendo) return;
 
                        std::string resposta = msg["resposta"];
 
                        if (resposta == "ACEITAR") {
                            estado.valorMao = valoresNivel[estado.nivelTruco];
                            estado.aguardandoRespostaTruco = false;
                            estado.ultimoEvento = "TRUCO_ACEITO";
 
                        } else if (resposta == "RECUSAR") {
                            // Quem pediu ganha valendo o nível anterior
                            int pontosGanhos = valoresNivel[estado.nivelTruco - 1];
                            if (estado.equipePedindo == 1) estado.pontosEq1 += pontosGanhos;
                            else estado.pontosEq2 += pontosGanhos;
 
                            estado.aguardandoRespostaTruco = false;
                            estado.ultimoEvento = "TRUCO_RECUSADO";
                            estado.equipeVencedora = estado.equipePedindo;
                            estado.vencedorMao = estado.equipePedindo;
 
                            // Verifica fim de partida
                            if (estado.pontosEq1 >= 12 || estado.pontosEq2 >= 12) {
                                estado.equipeVencedora = (estado.pontosEq1 >= 12) ? 1 : 2;
                                estado.ultimoEvento = "FIM_PARTIDA";
                                estado.partidaEncerrada = true;
                            } else {
                                estado.inicioRodada = (estado.inicioRodada + 1) % (int)estado.jogadores.size();
                                prepararRodada(estado);
                            }
 
                        } else if (resposta == "AUMENTAR") {
                            if (estado.nivelTruco >= 4) return;
                            // Inverte: agora a equipe que pediu precisa responder
                            std::swap(estado.equipePedindo, estado.equipeRespondendo);
                            estado.nivelTruco++;
                            estado.ultimoEvento = "TRUCO_PEDIDO";
                        }
                    }
 
                    // --- OBTER ESTADO ---
                    else if (acao == "OBTER_ESTADO") {
                        conn.send_text(construirEstado(estado).dump());
                        return;
                    }
 
                    // Broadcast do novo estado para todos
                    broadcast(estado, construirEstado(estado).dump());
 
                } catch (const std::exception& e) {
                    std::cerr << "Erro Truco: " << e.what() << std::endl;
                }
            });
 
        // Rotas estáticas (mesmo padrão dos outros servidores)
        CROW_ROUTE(app, "/")([]{
            crow::response res;
            res.set_static_file_info("frontend/pages/menu.html");
            return res;
        });
        CROW_ROUTE(app, "/pages/<path>")([](std::string p){
            crow::response res;
            res.set_static_file_info("frontend/pages/" + p);
            return res;
        });
        CROW_ROUTE(app, "/assets/<path>")([](std::string p){
            crow::response res;
            res.set_static_file_info("frontend/assets/" + p);
            return res;
        });
        CROW_ROUTE(app, "/css/<path>")([](std::string p){
            crow::response res;
            res.set_static_file_info("frontend/css/" + p);
            return res;
        });
        CROW_ROUTE(app, "/js/<path>")([](std::string p){
            crow::response res;
            res.set_static_file_info("frontend/js/" + p);
            return res;
        });
 
        app.port(8082).multithreaded().run();
 
    } catch (const std::exception& e) {
        std::cerr << "ERRO FATAL: " << e.what() << std::endl;
    }
    return 0;
}