#pragma once
#include <unordered_set>
#include <map>
#include <crow_all.h> // O compilador acha automático pelo -I
#include "MesaFDP.hpp" // Como estão na mesma pasta (include), não precisa de ../

class Servidor {
private:
    crow::SimpleApp app_; // O motor do servidor web
    MesaFDP* mesa_;       // Referência para o jogo que está rodando
    
    // Guarda as conexões ativas para podermos enviar mensagens de volta
    std::unordered_set<crow::websocket::connection*> conexoes_;
    
    // Opcional: Mapear a conexão para o ID do Jogador (Jogador 1, 2 ou 3)
    std::map<crow::websocket::connection*, int> idPorConexao_;

    // Função interna para disparar o JSON da mesa para todos os navegadores
    void notificarTodos();

public:
    // O servidor precisa nascer conhecendo a mesa
    Servidor(MesaFDP* mesa);
    
    // Configura as rotas e liga o servidor na porta escolhida
    void iniciar(int porta);
};