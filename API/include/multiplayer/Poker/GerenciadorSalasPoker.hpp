#pragma once

#include <map>
#include <memory>
#include <string>

#include "multiplayer/Poker/SalaPoker.hpp"

class GerenciadorSalasPoker {
private:
    static std::map<std::string, std::unique_ptr<SalaPoker>> salas_;

    static std::string gerarIdSala();

public:
    static SalaPoker* criarSala(int maxJogadores, ModoPoker modo);
    static SalaPoker* obterSala(const std::string& idSala);
    static bool salaExiste(const std::string& idSala);
    static bool removerSala(const std::string& idSala);
    static bool removerSalaSeVazia(const std::string& idSala);
};