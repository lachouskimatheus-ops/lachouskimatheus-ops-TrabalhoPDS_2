#include "dependencias/crow_all.h"
#include "dependencias/json.hpp"
#include "Jogos/Truco/include/Mesa.hpp"
#include "Jogos/Truco/include/BaralhoSujo.hpp"
#include "Jogos/Truco/include/JuizPaulista.hpp"
#include "Jogos/Truco/include/JuizMineiro.hpp"
#include "Jogos/Truco/include/Jogador_Truco.hpp"
 
#include <mutex>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
 
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
 
    // Configurações da Sala (LOBBY)
    bool salaConfigurada = false;
    int maxJogadores = 4;
    std::string modalidade = "paulista"; // "paulista" ou "mineiro"
 
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
    int valorAntesDoPedido = 1;
    bool maoTravada = false;
    std::string nomePedidor = "";

    // Vez
    int jogadorDaVez = 0;
    int inicioRodada = 0;

    bool partidaEncerrada = false;
    std::string ultimoEvento = "";
    int vencedorQueda = 0;
    int vencedorMao = 0;
    int equipeVencedora = 0;
    int pontosGanhosMao = 1;
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
 
    j["sala_configurada"] = estado.salaConfigurada;
    j["max_jogadores"]    = estado.maxJogadores;
    j["modalidade"]       = estado.modalidade;
 
    // Conta vagas disponíveis por equipe
    int eq1 = 0, eq2 = 0;
    for (const auto& jog : estado.jogadores) {
        if (jog.equipe == 1) eq1++;
        else if (jog.equipe == 2) eq2++;
    }
    j["vagas_eq1"] = (estado.maxJogadores / 2) - eq1;
    j["vagas_eq2"] = (estado.maxJogadores / 2) - eq2;
 
    j["pontos_equipe1"] = estado.pontosEq1;
    j["pontos_equipe2"] = estado.pontosEq2;
    j["valor_mao"]      = estado.valorMao;
    j["queda_atual"]    = estado.quedaAtual;
    j["nivel_truco"]    = estado.nivelTruco;
    j["mao_travada"]    = estado.maoTravada;
    j["aguardando_resposta_truco"] = estado.aguardandoRespostaTruco;
    j["equipe_respondendo"] = estado.equipeRespondendo;
    // Se há pedido pendente, o nome e valor são do nível atual (já incrementado)
    // Se não há pedido, mostra o próximo nível possível (para o botão)
    int nivelDisplay = estado.aguardandoRespostaTruco ? estado.nivelTruco : (estado.nivelTruco < 4 ? estado.nivelTruco + 1 : 4);
    j["nome_nivel_truco"] = nomesNivel[nivelDisplay];
    j["valor_se_aceito"]  = valoresNivel[nivelDisplay];
    j["nome_pedidor"]       = estado.nomePedidor;
 
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
    j["pontos_ganhos"]    = estado.pontosGanhosMao;
 
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
    estado.nivelTruco = 0;
    estado.quedaAtual = 1;
    estado.vitoriasEq1 = 0;
    estado.vitoriasEq2 = 0;
    estado.vencedorPrimeira = 0;
    estado.aguardandoRespostaTruco = false;
    estado.valorAntesDoPedido = 1;
    estado.nomePedidor = "";

    // Regra dos 11
    if (estado.pontosEq1 == 11 || estado.pontosEq2 == 11) {
        estado.valorMao = 3;
        estado.maoTravada = true;
    } else {
        estado.valorMao = 1;
        estado.maoTravada = false;
    }

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
        estado.pontosGanhosMao = estado.valorMao;

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
        estado.juiz    = new JuizPaulista(); // padrão temporário, atualizado ao criar sala

        std::mutex mtx;
        std::set<crow::websocket::connection*> conexoes;
        std::map<crow::websocket::connection*, int> connIds; // mapeia conn → jogadorId
        int proximoId = 1;
 
        CROW_WEBSOCKET_ROUTE(app, "/ws/truco")
            .onopen([&](crow::websocket::connection& conn) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.insert(&conn);
                // Estado será enviado após REGISTRAR ou RECONECTAR
            })
            .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
                std::lock_guard<std::mutex> lock(mtx);
                conexoes.erase(&conn);
                connIds.erase(&conn);
                
                // Remove o jogador se o jogo ainda não começou (Lobby)
                if (estado.jogadores.size() < estado.maxJogadores) {
                    estado.jogadores.erase(
                        std::remove_if(estado.jogadores.begin(), estado.jogadores.end(),
                        [&](const JogadorConectado& j) { return j.conn == &conn; }),
                        estado.jogadores.end()
                    );
                    // Se a sala esvaziou no lobby, reseta a configuração
                    if (estado.jogadores.empty()) {
                        estado.salaConfigurada = false;
                    }
                    broadcast(estado, construirEstado(estado).dump());
                } else {
                    // Apenas desconecta o ponteiro se o jogo já estiver rolando
                    for (auto& j : estado.jogadores) {
                        if (j.conn == &conn) j.conn = nullptr;
                    }
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

                    // --- REGISTRAR (novo jogador sem ID) ---
                    if (acao == "REGISTRAR") {
                        // Atribui o próximo ID disponível
                        int novoId = proximoId++;
                        std::string nome = msg.value("nome", "Jogador " + std::to_string(novoId));

                        // Guarda conn->id para uso futuro
                        connIds[&conn] = novoId;

                        // Responde só para este jogador com seu ID
                        json resp = construirEstado(estado);
                        resp["meu_id"] = novoId;
                        conn.send_text(resp.dump());
                        return;
                    }

                    // --- RECONECTAR (jogador com ID já conhecido) ---
                    if (acao == "RECONECTAR") {
                        int id = msg.value("jogador_id", -1);
                        if (id > 0) {
                            connIds[&conn] = id;
                            // Atualiza conn do jogador se já estava na lista
                            for (auto& j : estado.jogadores) {
                                if (j.id == id) { j.conn = &conn; break; }
                            }
                        }
                        json resp = construirEstado(estado);
                        resp["meu_id"] = id;
                        conn.send_text(resp.dump());
                        return;
                    }

                    // Resolve jogadorId: usa o da mensagem ou o mapeado pela conn
                    if (jogadorId <= 0) {
                        auto it = connIds.find(&conn);
                        if (it != connIds.end()) jogadorId = it->second;
                    }

                    // --- CRIAR SALA (HOST) ---
                    if (acao == "CRIAR_SALA") {
                        if (estado.salaConfigurada) return;

                        estado.maxJogadores = msg.value("max_jogadores", 4);
                        estado.modalidade = msg.value("modalidade", "paulista");
                        estado.salaConfigurada = true;

                        if (estado.juiz) delete estado.juiz;
                        if (estado.modalidade == "mineiro") estado.juiz = new JuizMineiro();
                        else estado.juiz = new JuizPaulista();

                        int equipeEscolhida = msg.value("equipe", 1);
                        std::string nome = msg.value("nome", "Jogador " + std::to_string(jogadorId));

                        JogadorConectado jog;
                        jog.id = jogadorId;
                        jog.nome = nome;
                        jog.equipe = equipeEscolhida;
                        jog.conn = &conn;
                        estado.jogadores.push_back(jog);

                        broadcast(estado, construirEstado(estado).dump());
                    }
 
                    // --- ENTRAR (CONVIDADOS) ---
                    else if (acao == "ENTRAR") {
                        if (!estado.salaConfigurada) return; // Sala não existe ainda
                        if (estado.jogadores.size() >= estado.maxJogadores) return; // Sala cheia
 
                        // Verifica se já está na sala
                        for (auto& j : estado.jogadores) if (j.id == jogadorId) return;
 
                        int equipeEscolhida = msg.value("equipe", 1);
                        
                        // Validação de segurança para não ultrapassar vagas
                        int eq1 = 0, eq2 = 0;
                        for (const auto& jg : estado.jogadores) {
                            if (jg.equipe == 1) eq1++;
                            else eq2++;
                        }
                        int limite = estado.maxJogadores / 2;
                        if (equipeEscolhida == 1 && eq1 >= limite) equipeEscolhida = 2;
                        if (equipeEscolhida == 2 && eq2 >= limite) equipeEscolhida = 1;
 
                        std::string nome = msg.value("nome", "Jogador " + std::to_string(jogadorId));
                        JogadorConectado jog;
                        jog.id = jogadorId;
                        jog.nome = nome;
                        jog.equipe = equipeEscolhida;
                        jog.conn = &conn;
                        estado.jogadores.push_back(jog);
 
                        // Inicia o jogo se lotou
                        if (estado.jogadores.size() == estado.maxJogadores) {
                            prepararRodada(estado);
                        }
                        broadcast(estado, construirEstado(estado).dump());
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
                        if (estado.maoTravada) return;

                        int idxJogador = -1;
                        for (int i = 0; i < (int)estado.jogadores.size(); i++) {
                            if (estado.jogadores[i].id == jogadorId) { idxJogador = i; break; }
                        }
                        if (idxJogador != estado.jogadorDaVez) return;

                        estado.valorAntesDoPedido = estado.valorMao;
                        estado.nivelTruco++;
                        estado.equipePedindo     = getEquipe(idxJogador);
                        estado.equipeRespondendo = (estado.equipePedindo == 1) ? 2 : 1;
                        estado.aguardandoRespostaTruco = true;
                        estado.nomePedidor = estado.jogadores[idxJogador].nome;
                        estado.ultimoEvento = "TRUCO_PEDIDO";
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
                            int pontosGanhos = estado.valorAntesDoPedido;
                            if (estado.equipePedindo == 1) estado.pontosEq1 += pontosGanhos;
                            else estado.pontosEq2 += pontosGanhos;

                            estado.pontosGanhosMao = pontosGanhos;
                            estado.aguardandoRespostaTruco = false;
                            estado.ultimoEvento = "TRUCO_RECUSADO";
                            estado.equipeVencedora = estado.equipePedindo;
                            estado.vencedorMao = estado.equipePedindo;

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
                            estado.valorAntesDoPedido = valoresNivel[estado.nivelTruco];
                            std::swap(estado.equipePedindo, estado.equipeRespondendo);
                            estado.nivelTruco++;
                            // Atualiza nome de quem está pedindo agora
                            for (int i = 0; i < (int)estado.jogadores.size(); i++) {
                                if (getEquipe(i) == estado.equipePedindo) {
                                    estado.nomePedidor = estado.jogadores[i].nome; break;
                                }
                            }
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