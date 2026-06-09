#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "multiplayer/FDP/SalaFDP.hpp"

class GerenciadorSalasFDP {
private:
    static std::map<
        std::string,
        std::shared_ptr<SalaFDP>
    > salas_;

    static std::mutex mutexSalas_;

    static std::string gerarIdSala();

public:
    static std::shared_ptr<SalaFDP> criarSala(
        int maxJogadores
    );

    static std::shared_ptr<SalaFDP> obterSala(
        const std::string& idSala
    );

    static bool salaExiste(
        const std::string& idSala
    );

    static bool removerSala(
        const std::string& idSala
    );

    static bool removerSalaSeVazia(
        const std::string& idSala
    );

    static std::vector<std::string>
    listarIdsSalas();
};