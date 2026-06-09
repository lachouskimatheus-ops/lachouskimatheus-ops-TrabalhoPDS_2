#pragma once

#include <map>
#include <memory>
#include <string>
#include "multiplayer/Truco/SalaTruco.hpp"

class GerenciadorSalasTruco {
private:
    static std::map<std::string, std::unique_ptr<SalaTruco>> salas_;

    static std::string gerarIdSala();

public:
    static SalaTruco* criarSala(TipoTruco tipo, int maxJogadores);
    static SalaTruco* obterSala(const std::string& idSala);
    static bool salaExiste(const std::string& idSala);
    static bool removerSala(const std::string& idSala);
};