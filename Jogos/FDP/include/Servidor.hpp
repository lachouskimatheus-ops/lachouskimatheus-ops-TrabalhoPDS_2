#pragma once
#include <map>
#include <crow_all.h> // O compilador acha automático pelo -I
#include "MesaFDP.hpp" // Como estão na mesma pasta (include), não precisa de ../

class Servidor {
private:
    crow::SimpleApp app_; // O motor do servidor web
    MesaFDP* mesa_;       // Referência para o jogo que está rodando
    
    // A MÁGICA AQUI: O mapa substitui o unordered_set antigo, 
    // mas mantém o mesmo nome "conexoes_" para não quebrar o resto do código.
    // Ele liga a conexão (aba do navegador) ao ID do Jogador (0, 1, 2 ou 3)
    std::map<crow::websocket::connection*, int> conexoes_;

    // Função interna para disparar o JSON da mesa para todos os navegadores
    void notificarTodos();

public:
    // O servidor precisa nascer conhecendo a mesa
    Servidor(MesaFDP* mesa);
    
    // Configura as rotas e liga o servidor na porta escolhida
    void iniciar(int porta);
};