#include "routes/FDPRoutes.hpp"
#include "multiplayer/FDP/SalaFDP.hpp"

#include "json.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

using json = nlohmann::json;

/*
 * A SalaFDP é responsável por:
 *
 * - possuir o baralho;
 * - possuir o placar;
 * - possuir a MesaFDP;
 * - criar os jogadores;
 * - iniciar a partida;
 * - validar de quem é a vez;
 * - gerar o JSON individual de cada jogador.
 *
 * Portanto, FDPRoutes não deve criar outra MesaFDP separada.
 */
SalaFDP salaFDP;

/*
 * Relaciona cada conexão WebSocket ao ID do jogador.
 *
 * Enquanto o jogador ainda não enviou ENTRAR, seu ID permanece como -1.
 */
std::unordered_map<crow::websocket::connection*, int> conexoesFDP;

/*
 * Crow pode processar conexões em threads diferentes.
 *
 * Além disso, existe uma thread usada para aguardar a exibição da vaza.
 * Por isso, o acesso à sala e ao mapa de conexões deve ser protegido.
 */
std::mutex mutexFDP;


/*
 * Envia uma mensagem somente para uma conexão.
 */
void enviarMensagem(
    crow::websocket::connection& conn,
    const json& mensagem
) {
    try {
        conn.send_text(mensagem.dump());
    } catch (const std::exception& erro) {
        std::cerr
            << "[FDP] Erro ao enviar mensagem: "
            << erro.what()
            << '\n';
    }
}


/*
 * Envia uma mensagem de erro para o jogador que realizou a ação.
 */
void enviarErro(
    crow::websocket::connection& conn,
    const std::string& mensagem
) {
    json resposta = {
        {"tipo", "ERRO"},
        {"mensagem", mensagem}
    };

    enviarMensagem(conn, resposta);
}


/*
 * Gera o estado individual de cada jogador.
 *
 * Cada jogador precisa receber seu próprio JSON porque somente ele pode
 * visualizar as cartas da própria mão.
 *
 * Os JSONs são produzidos enquanto a sala está protegida pelo mutex.
 * Depois, o envio ocorre fora do mutex para não manter o jogo bloqueado
 * durante operações de rede.
 */
void notificarTodosFDP() {
    std::vector<
        std::pair<crow::websocket::connection*, std::string>
    > mensagens;

    {
        std::lock_guard<std::mutex> lock(mutexFDP);

        mensagens.reserve(conexoesFDP.size());

        for (const auto& par : conexoesFDP) {
            crow::websocket::connection* conexao = par.first;
            int idJogador = par.second;

            if (conexao == nullptr || idJogador < 0) {
                continue;
            }

            try {
                json estado = salaFDP.gerarJson(idJogador);

                mensagens.emplace_back(
                    conexao,
                    estado.dump()
                );
            } catch (const std::exception& erro) {
                std::cerr
                    << "[FDP] Erro ao gerar JSON para o jogador "
                    << idJogador
                    << ": "
                    << erro.what()
                    << '\n';
            }
        }
    }

    for (const auto& mensagem : mensagens) {
        crow::websocket::connection* conexao = mensagem.first;

        if (conexao == nullptr) {
            continue;
        }

        try {
            conexao->send_text(mensagem.second);
        } catch (const std::exception& erro) {
            std::cerr
                << "[FDP] Erro ao notificar jogador: "
                << erro.what()
                << '\n';
        }
    }
}


/*
 * Aguarda a animação das cartas da vaza e depois avança o jogo.
 */
void finalizarVazaDepoisDoAtraso() {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(2500)
    );

    {
        std::lock_guard<std::mutex> lock(mutexFDP);

        /*
         * Confere novamente porque o estado pode ter mudado durante
         * o tempo de espera.
         */
        if (!salaFDP.vazaFinalizada()) {
            return;
        }

        salaFDP.finalizarVaza();

        if (salaFDP.rodadaFinalizada()) {
            salaFDP.finalizarRodada();
        }
    }

    notificarTodosFDP();
}

} // namespace


void FDPRoutes::registrar(crow::SimpleApp& app) {

    CROW_WEBSOCKET_ROUTE(app, "/ws/fdp")

    .onopen([](crow::websocket::connection& conn) {
        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            /*
             * O ID ainda é desconhecido.
             * Ele será associado quando chegar a ação ENTRAR.
             */
            conexoesFDP[&conn] = -1;
        }

        std::cout
            << "[FDP] Nova conexão WebSocket."
            << std::endl;
    })

    .onclose([](
        crow::websocket::connection& conn,
        const std::string& motivo,
        std::uint16_t codigo
    ) {
        int idJogador = -1;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            auto it = conexoesFDP.find(&conn);

            if (it != conexoesFDP.end()) {
                idJogador = it->second;
                conexoesFDP.erase(it);
            }

            if (idJogador >= 0) {
                salaFDP.desconectarJogador(idJogador);
            }
        }

        std::cout
            << "[FDP] Conexão encerrada";

        if (idJogador >= 0) {
            std::cout
                << " — jogador "
                << idJogador;
        }

        std::cout
            << ". Código: "
            << codigo;

        if (!motivo.empty()) {
            std::cout
                << ". Motivo: "
                << motivo;
        }

        std::cout << std::endl;

        /*
         * Atualiza os demais jogadores após a desconexão.
         */
        notificarTodosFDP();
    })

    .onmessage([](
        crow::websocket::connection& conn,
        const std::string& data,
        bool mensagemBinaria
    ) {
        /*
         * O protocolo do jogo utiliza JSON em texto.
         */
        if (mensagemBinaria) {
            enviarErro(
                conn,
                "O servidor do FDP aceita apenas mensagens de texto."
            );
            return;
        }

        try {
            json comando = json::parse(data);

            if (!comando.is_object()) {
                enviarErro(
                    conn,
                    "O comando recebido não é um objeto JSON."
                );
                return;
            }

            if (
                !comando.contains("acao") ||
                !comando["acao"].is_string()
            ) {
                enviarErro(
                    conn,
                    "O campo 'acao' não foi informado corretamente."
                );
                return;
            }

            if (
                !comando.contains("jogador_id") ||
                !comando["jogador_id"].is_number_integer()
            ) {
                enviarErro(
                    conn,
                    "O campo 'jogador_id' não foi informado corretamente."
                );
                return;
            }

            const std::string acao =
                comando["acao"].get<std::string>();

            const int idRemetente =
                comando["jogador_id"].get<int>();

            /*
             * A SalaFDP atualmente cria jogadores com IDs de 0 a 3.
             */
            if (idRemetente < 0 || idRemetente > 3) {
                enviarErro(
                    conn,
                    "ID de jogador inválido. O FDP aceita IDs de 0 a 3."
                );
                return;
            }

            /*
             * ENTRAR
             */
            if (acao == "ENTRAR") {
                {
                    std::lock_guard<std::mutex> lock(mutexFDP);

                    /*
                     * Evita que a mesma conexão troque de jogador depois
                     * que já entrou na sala.
                     */
                    auto conexaoAtual = conexoesFDP.find(&conn);

                    if (
                        conexaoAtual != conexoesFDP.end() &&
                        conexaoAtual->second >= 0 &&
                        conexaoAtual->second != idRemetente
                    ) {
                        enviarErro(
                            conn,
                            "Esta conexão já pertence a outro jogador."
                        );
                        return;
                    }

                    /*
                     * Impede duas conexões de assumirem o mesmo jogador.
                     */
                    for (const auto& par : conexoesFDP) {
                        if (
                            par.first != &conn &&
                            par.second == idRemetente
                        ) {
                            enviarErro(
                                conn,
                                "Esse jogador já está conectado."
                            );
                            return;
                        }
                    }

                    conexoesFDP[&conn] = idRemetente;
                    salaFDP.conectarJogador(idRemetente);
                }

                std::cout
                    << "[FDP] Jogador "
                    << idRemetente
                    << " entrou na sala."
                    << std::endl;

                notificarTodosFDP();
                return;
            }

            /*
             * Nenhuma ação do jogo pode ser processada antes de ENTRAR.
             */
            {
                std::lock_guard<std::mutex> lock(mutexFDP);

                auto it = conexoesFDP.find(&conn);

                if (
                    it == conexoesFDP.end() ||
                    it->second < 0
                ) {
                    enviarErro(
                        conn,
                        "Envie a ação ENTRAR antes de jogar."
                    );
                    return;
                }

                if (it->second != idRemetente) {
                    enviarErro(
                        conn,
                        "O jogador_id não corresponde a esta conexão."
                    );
                    return;
                }
            }

            /*
             * APOSTAR
             */
            if (acao == "APOSTAR") {
                if (
                    !comando.contains("valor") ||
                    !comando["valor"].is_number_integer()
                ) {
                    enviarErro(
                        conn,
                        "O campo 'valor' da aposta é inválido."
                    );
                    return;
                }

                const int valorAposta =
                    comando["valor"].get<int>();

                bool apostaRealizada = false;

                {
                    std::lock_guard<std::mutex> lock(mutexFDP);

                    apostaRealizada =
                        salaFDP.apostar(
                            idRemetente,
                            valorAposta
                        );
                }

                if (!apostaRealizada) {
                    enviarErro(
                        conn,
                        "A aposta não pôde ser realizada. Verifique a vez e o valor."
                    );
                    return;
                }

                notificarTodosFDP();
                return;
            }

            /*
             * JOGAR_CARTA
             */
            if (acao == "JOGAR_CARTA") {
                if (
                    !comando.contains("indice") ||
                    !comando["indice"].is_number_integer()
                ) {
                    enviarErro(
                        conn,
                        "O campo 'indice' da carta é inválido."
                    );
                    return;
                }

                const int indiceCarta =
                    comando["indice"].get<int>();

                bool cartaJogada = false;
                bool terminouVaza = false;

                {
                    std::lock_guard<std::mutex> lock(mutexFDP);

                    cartaJogada =
                        salaFDP.jogarCarta(
                            idRemetente,
                            indiceCarta
                        );

                    if (cartaJogada) {
                        terminouVaza =
                            salaFDP.vazaFinalizada();
                    }
                }

                if (!cartaJogada) {
                    enviarErro(
                        conn,
                        "A carta não pôde ser jogada. Verifique a vez e o índice."
                    );
                    return;
                }

                /*
                 * Primeiro mostra a carta jogada para todos.
                 */
                notificarTodosFDP();

                /*
                 * Quando a vaza termina, espera o frontend exibir as
                 * quatro cartas antes de apurar o vencedor.
                 */
                if (terminouVaza) {
                    std::thread(
                        finalizarVazaDepoisDoAtraso
                    ).detach();
                }

                return;
            }

            /*
             * ATUALIZAR_ESTADO pode ser usado pelo frontend para solicitar
             * novamente o estado sem alterar a partida.
             */
            if (
                acao == "ATUALIZAR_ESTADO" ||
                acao == "OBTER_ESTADO"
            ) {
                json estado;

                {
                    std::lock_guard<std::mutex> lock(mutexFDP);
                    estado = salaFDP.gerarJson(idRemetente);
                }

                enviarMensagem(conn, estado);
                return;
            }

            enviarErro(
                conn,
                "Ação desconhecida: " + acao
            );

        } catch (const json::parse_error& erro) {
            std::cerr
                << "[FDP] JSON inválido: "
                << erro.what()
                << std::endl;

            enviarErro(
                conn,
                "O servidor recebeu um JSON inválido."
            );

        } catch (const json::exception& erro) {
            std::cerr
                << "[FDP] Erro ao ler JSON: "
                << erro.what()
                << std::endl;

            enviarErro(
                conn,
                "Os dados do comando possuem formato inválido."
            );

        } catch (const std::exception& erro) {
            std::cerr
                << "[FDP] Erro interno: "
                << erro.what()
                << std::endl;

            enviarErro(
                conn,
                "Ocorreu um erro interno ao processar a ação."
            );
        }
    });
}