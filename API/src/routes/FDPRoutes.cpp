#include "routes/FDPRoutes.hpp"
#include "multiplayer/FDP/GerenciadorSalasFDP.hpp"
#include "json.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

    using json = nlohmann::json;

    struct SessaoFDP {
        std::string idSala;
        int idJogador;
        std::string tokenReconexao;
    };

    std::unordered_map<crow::websocket::connection*, SessaoFDP> sessoesFDP;
    std::mutex mutexFDP;

    void enviarMensagem(crow::websocket::connection& conexao, const json& mensagem) {
        try {
            conexao.send_text(mensagem.dump());
        } catch (const std::exception& erro) {
            std::cerr << "[FDP] Erro ao enviar mensagem: " << erro.what() << '\n';
        };
    };

    void enviarErro(crow::websocket::connection& conexao, const std::string& mensagem) {
        enviarMensagem(conexao, {
            {"tipo", "ERRO"},
            {"erro", mensagem},
            {"mensagem", mensagem}
        });
    };

    bool tokenValido(const std::string& token) {
        return token.size() >= 8 && token.size() <= 100;
    };

    void notificarSala(const std::shared_ptr<SalaFDP>& sala) {
        if (sala == nullptr) { return; };

        std::vector<std::pair<crow::websocket::connection*, std::string>> mensagens;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            for (const ConexaoFDP& registro : sala->conexoes()) {
                if (registro.conexao == nullptr) { continue; };
                if (!sala->jogadorEstaConectado(registro.idJogador)) { continue; };

                try {
                    mensagens.emplace_back(
                        registro.conexao,
                        sala->gerarJson(registro.idJogador).dump()
                    );
                } catch (const std::exception& erro) {
                    std::cerr << "[FDP] Erro ao gerar estado: " << erro.what() << '\n';
                };
            };
        };

        for (const auto& mensagem : mensagens) {
            if (mensagem.first == nullptr) { continue; };

            try {
                mensagem.first->send_text(mensagem.second);
            } catch (const std::exception& erro) {
                std::cerr << "[FDP] Erro ao notificar sala: " << erro.what() << '\n';
            };
        };
    };

    void finalizarVazaDepoisDoAtraso(std::shared_ptr<SalaFDP> sala) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            if (sala == nullptr) { return; };

            if (!sala->vazaFinalizada()) {
                sala->definirProcessandoFimVaza(false);
                return;
            };

            sala->finalizarVaza();

            if (sala->rodadaFinalizada()) {
                sala->finalizarRodada();
            };

            sala->definirProcessandoFimVaza(false);
        };

        notificarSala(sala);
    };

    void verificarFimDaVaza(const std::shared_ptr<SalaFDP>& sala) {
        bool criarThread = false;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            if (sala != nullptr &&
                sala->vazaFinalizada() &&
                !sala->processandoFimVaza()) {
                sala->definirProcessandoFimVaza(true);
                criarThread = true;
            };
        };

        if (criarThread) {
            std::thread(finalizarVazaDepoisDoAtraso, sala).detach();
        };
    };

    void entrarNaSala(crow::websocket::connection& conexao, const json& comando) {
        if (!comando.contains("sala") || !comando["sala"].is_string()) {
            enviarErro(conexao, "Informe o código da sala.");
            return;
        };

        if (!comando.contains("token") || !comando["token"].is_string()) {
            enviarErro(conexao, "Informe o token de reconexão.");
            return;
        };

        std::string idSala = comando["sala"].get<std::string>();
        std::string token = comando["token"].get<std::string>();

        // ---> NOVA LÓGICA DE NOME AQUI <---
        std::string nomeRecebido = "";
        if (comando.contains("nome") && comando["nome"].is_string()) {
            nomeRecebido = comando["nome"].get<std::string>();
        };

        if (!tokenValido(token)) {
            enviarErro(conexao, "Token de reconexão inválido.");
            return;
        };

        std::shared_ptr<SalaFDP> sala = GerenciadorSalasFDP::obterSala(idSala);

        if (sala == nullptr) {
            enviarErro(conexao, "Sala não encontrada.");
            return;
        };

        bool reconexao = false;
        int idJogador = -1;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            if (sessoesFDP.find(&conexao) != sessoesFDP.end()) {
                enviarErro(conexao, "Esta conexão já entrou em uma sala.");
                return;
            };

            reconexao = sala->podeReconectar(token);

            if (sala->partidaIniciada() && !reconexao) {
                enviarErro(conexao, "A partida já foi iniciada.");
                return;
            };

            if (!reconexao && !sala->podeReceberNovoJogador()) {
                enviarErro(conexao, "A sala está cheia.");
                return;
            };

            crow::websocket::connection* conexaoAnterior = nullptr;

            if (reconexao) {
                int idAnterior = sala->buscarJogadorPorToken(token);
                conexaoAnterior = sala->obterConexaoJogador(idAnterior);
            };

            idJogador = sala->adicionarJogador(&conexao, token);

            if (idJogador == -1) {
                enviarErro(conexao, "Não foi possível entrar na sala.");
                return;
            };

            // ---> APLICANDO O NOME NO BACKEND <---
            if (!nomeRecebido.empty()) {
                // OBSERVAÇÃO: Você precisa garantir que este método exista dentro da sua classe SalaFDP!
                // sala->definirNomeJogador(idJogador, nomeRecebido);
            };

            if (conexaoAnterior != nullptr && conexaoAnterior != &conexao) {
                sessoesFDP.erase(conexaoAnterior);
            };

            sessoesFDP[&conexao] = {idSala, idJogador, token};
        };

        enviarMensagem(conexao, {
            {
                "tipo",
                reconexao ? "RECONEXAO_CONFIRMADA" : "ENTRADA_CONFIRMADA"
            },
            {"sala", idSala},
            {"jogador_id", idJogador},
            {"meu_id", idJogador},
            {"max_jogadores", sala->maxJogadores()},
            {"partida_iniciada", sala->partidaIniciada()}
        });

        notificarSala(sala);
    };

    void processarAcao(crow::websocket::connection& conexao, const json& comando) {
        SessaoFDP sessao;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            auto it = sessoesFDP.find(&conexao);

            if (it == sessoesFDP.end()) {
                enviarErro(conexao, "Você ainda não entrou em uma sala.");
                return;
            };

            sessao = it->second;
        };

        std::shared_ptr<SalaFDP> sala =
            GerenciadorSalasFDP::obterSala(sessao.idSala);

        if (sala == nullptr) {
            enviarErro(conexao, "Sala não encontrada.");
            return;
        };

        if (!sala->partidaIniciada()) {
            enviarErro(conexao, "Aguardando os outros jogadores.");
            notificarSala(sala);
            return;
        };

        if (!comando.contains("acao") || !comando["acao"].is_string()) {
            enviarErro(conexao, "Informe a ação do jogo.");
            return;
        };

        std::string acao = comando["acao"].get<std::string>();
        bool sucesso = false;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            if (acao == "APOSTAR") {
                if (!comando.contains("valor") ||
                    !comando["valor"].is_number_integer()) {
                    enviarErro(conexao, "Informe o valor da aposta.");
                    return;
                };

                sucesso = sala->apostar(
                    sessao.idJogador,
                    comando["valor"].get<int>()
                );
            } else if (acao == "JOGAR_CARTA") {
                if (!comando.contains("indice") ||
                    !comando["indice"].is_number_integer()) {
                    enviarErro(conexao, "Informe o índice da carta.");
                    return;
                };

                sucesso = sala->jogarCarta(
                    sessao.idJogador,
                    comando["indice"].get<int>()
                );
            } else {
                enviarErro(conexao, "Ação do FDP não reconhecida.");
                return;
            };
        };

        if (!sucesso) {
            enviarErro(conexao, "A ação não pôde ser realizada.");
            notificarSala(sala);
            return;
        };

        notificarSala(sala);

        if (acao == "JOGAR_CARTA") {
            verificarFimDaVaza(sala);
        };
    };

    void obterEstado(crow::websocket::connection& conexao) {
        SessaoFDP sessao;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            auto it = sessoesFDP.find(&conexao);

            if (it == sessoesFDP.end()) {
                enviarErro(conexao, "Você ainda não entrou em uma sala.");
                return;
            };

            sessao = it->second;
        };

        std::shared_ptr<SalaFDP> sala =
            GerenciadorSalasFDP::obterSala(sessao.idSala);

        if (sala == nullptr) {
            enviarErro(conexao, "Sala não encontrada.");
            return;
        };

        json estado;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);
            estado = sala->gerarJson(sessao.idJogador);
        };

        enviarMensagem(conexao, estado);
    };

}

void FDPRoutes::registrar(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/fdp/salas")
    .methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        try {
            json corpo = json::parse(req.body);

            if (!corpo.contains("jogadores") ||
                !corpo["jogadores"].is_number_integer()) {
                return crow::response(
                    400,
                    json{
                        {"erro", "Informe a quantidade de jogadores."}
                    }.dump()
                );
            };

            int quantidade = corpo["jogadores"].get<int>();

            std::shared_ptr<SalaFDP> sala =
                GerenciadorSalasFDP::criarSala(quantidade);

            if (sala == nullptr) {
                return crow::response(
                    400,
                    json{
                        {"erro", "O FDP aceita de 2 a 4 jogadores."}
                    }.dump()
                );
            };

            crow::response resposta(
                201,
                json{
                    {"sala", sala->idSala()},
                    {"max_jogadores", sala->maxJogadores()}
                }.dump()
            );

            resposta.set_header("Content-Type", "application/json");
            return resposta;
        } catch (const std::exception&) {
            return crow::response(
                400,
                json{
                    {"erro", "JSON inválido."}
                }.dump()
            );
        };
    });

    CROW_WEBSOCKET_ROUTE(app, "/ws/fdp")

    .onopen([](crow::websocket::connection& conexao) {
        enviarMensagem(conexao, {
            {"tipo", "CONECTADO"},
            {"mensagem", "Conectado ao servidor do FDP."}
        });
    })

    .onmessage([](
        crow::websocket::connection& conexao,
        const std::string& dados,
        bool binaria
    ) {
        if (binaria) {
            enviarErro(conexao, "Mensagens binárias não são aceitas.");
            return;
        };

        try {
            json comando = json::parse(dados);

            if (!comando.is_object() ||
                !comando.contains("tipo") ||
                !comando["tipo"].is_string()) {
                enviarErro(conexao, "Mensagem JSON inválida.");
                return;
            };

            std::string tipo = comando["tipo"].get<std::string>();

            if (tipo == "ENTRAR_SALA") {
                entrarNaSala(conexao, comando);
            } else if (tipo == "ACAO_JOGO") {
                processarAcao(conexao, comando);
            } else if (tipo == "OBTER_ESTADO") {
                obterEstado(conexao);
            } else if (tipo == "PING") {
                enviarMensagem(conexao, {{"tipo", "PONG"}});
            } else {
                enviarErro(conexao, "Tipo de mensagem não reconhecido.");
            };
        } catch (const std::exception& erro) {
            enviarErro(
                conexao,
                std::string("Erro ao processar mensagem: ") + erro.what()
            );
        };
    })

    .onclose([](
        crow::websocket::connection& conexao,
        const std::string& motivo,
        std::uint16_t codigo
    ) {
        std::shared_ptr<SalaFDP> sala;
        std::string idSala;

        {
            std::lock_guard<std::mutex> lock(mutexFDP);

            auto it = sessoesFDP.find(&conexao);

            if (it != sessoesFDP.end()) {
                idSala = it->second.idSala;
                sala = GerenciadorSalasFDP::obterSala(idSala);

                if (sala != nullptr) {
                    sala->removerConexao(&conexao);
                };

                sessoesFDP.erase(it);
            };
        };

        std::cout << "[FDP] Conexão encerrada. Código: " << codigo;

        if (!motivo.empty()) {
            std::cout << ". Motivo: " << motivo;
        };

        std::cout << '\n';

        if (sala != nullptr) {
            notificarSala(sala);

            if (sala->estaVazia() && !sala->partidaIniciada()) {
                GerenciadorSalasFDP::removerSalaSeVazia(idSala);
            };
        };
    });
}