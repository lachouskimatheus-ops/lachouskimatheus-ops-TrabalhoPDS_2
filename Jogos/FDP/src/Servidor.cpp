#include "Servidor.hpp" 
#include "json.hpp"
#include <iostream>
#include <thread>
#include <chrono>

Servidor::Servidor(MesaFDP* mesa) {
    mesa_ = mesa;
};

void Servidor::notificarTodos() {
    // 1. Pega o estado completo da mesa
    std::string estadoJson = mesa_->paraJson().dump();
    
    // 2. Dispara essa string para todos os jogadores conectados
    for (auto* conn : conexoes_) {
        conn->send_text(estadoJson);
    };
};

void Servidor::iniciar(int porta) {
    // Definindo a rota do WebSocket
    CROW_WEBSOCKET_ROUTE(app_, "/ws")
      .onopen([this](crow::websocket::connection& conn) {
          std::cout << "Novo jogador conectado!" << std::endl;
          conexoes_.insert(&conn);
          
          // Sempre que alguém entra, mandamos o estado atual da mesa
          notificarTodos();
      })
      .onclose([this](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
          std::cout << "Jogador desconectado!" << std::endl;
          conexoes_.erase(&conn);
      })
      .onmessage([this](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
          std::cout << "\n========================================" << std::endl;
          std::cout << "[DEBUG SERVER] Recebi uma mensagem crua: " << data << std::endl;
          
          try {
              auto comando = nlohmann::json::parse(data);
              
              if (!comando.contains("jogador_id") || !comando.contains("acao")) {
                  std::cout << "[DEBUG SERVER] Erro: Mensagem invalida (sem ID ou acao)." << std::endl;
                  return;
              };

              int idRemetente = comando["jogador_id"];
              std::string acao = comando["acao"];

              // SEGURANÇA: Só processa o comando se o ID de quem enviou for igual ao jogador da vez no C++
              if (idRemetente != mesa_->getJogadorDaVezIndex()) {
                  std::cout << "[SEGURANCA] Jogador " << idRemetente << " tentou jogar fora da sua vez!" << std::endl;
                  return; 
              };

              // ==========================================
              // BLOCO: JOGAR CARTA
              // ==========================================
              if (acao == "JOGAR_CARTA") {
                  int indiceCarta = comando["indice"];
                  std::cout << "[DEBUG] Jogador " << idRemetente << " tentou jogar o indice: " << indiceCarta << std::endl;
                  
                  if (mesa_->jogarCarta(indiceCarta)) {
                      std::cout << "[DEBUG] Carta aceita e colocada na mesa." << std::endl;
                      
                      // 1. Atualiza os navegadores imediatamente para a carta aparecer na mesa
                      notificarTodos();
                      
                      // 2. Verifica se a vaza acabou para iniciar a pausa dramática
                      if (mesa_->vazaFinalizada()) {
                          std::cout << "[DEBUG] Vaza finalizada! Iniciando a pausa para visualizacao..." << std::endl;
                          
                          // Cria uma thread paralela para não congelar o servidor
                          std::thread([this]() {
                              // Pausa de 2.5 segundos
                              std::this_thread::sleep_for(std::chrono::milliseconds(2500));
                              
                              // Apura a vaza e limpa a mesa
                              mesa_->apurarVencedorDaVaza();
                              std::cout << "[DEBUG THREAD] Vencedor apurado e mesa limpa." << std::endl;
                              
                              // Verifica se as mãos esvaziaram (a rodada INTEIRA acabou)
                              if (mesa_->rodadaFinalizada()) {
                                  std::cout << "[DEBUG THREAD] Maos vazias! Fim da Rodada. Chamando finalizarRodada()..." << std::endl;
                                  mesa_->finalizarRodada();
                              } else {
                                  std::cout << "[DEBUG THREAD] A vaza acabou, mas a rodada de cartas continua." << std::endl;
                              };
                              
                              // Atualiza os navegadores novamente com a mesa limpa ou cartas novas
                              notificarTodos();
                          }).detach();
                      };
                  } else {
                      std::cout << "[DEBUG] ERRO: Jogada foi rejeitada pela classe MesaFDP." << std::endl;
                  };
              }
              // ==========================================
              // BLOCO: APOSTAR
              // ==========================================
              else if (acao == "APOSTAR") {
                  int valorAposta = comando["valor"];
                  std::cout << "[DEBUG] Jogador " << idRemetente << " apostou: " << valorAposta << std::endl;
                  
                  if (mesa_->registrarAposta(valorAposta)) {
                      if (mesa_->faseApostasFinalizada()) {
                          std::cout << "[DEBUG] Fase de apostas acabou. Liberando as cartas..." << std::endl;
                          mesa_->iniciarFaseDeCartas();
                      };
                      notificarTodos();
                  };
              } else {
                  std::cout << "[DEBUG] Acao desconhecida: " << acao << std::endl;
              };
              
          } catch (const std::exception& e) {
              std::cout << "[ERRO CRITICO] Falha ao ler JSON: " << e.what() << std::endl;
          };
      });

    std::cout << "Iniciando servidor na porta " << porta << "..." << std::endl;
    app_.port(porta).multithreaded().run();
};