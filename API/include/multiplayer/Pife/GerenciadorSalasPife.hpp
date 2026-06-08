#ifndef GERENCIADOR_SALAS_PIFE_HPP
#define GERENCIADOR_SALAS_PIFE_HPP

#include <map>
#include <memory>
#include <string>

#include "multiplayer/Pife/SalaPife.hpp"

class GerenciadorSalasPife {
private:
    static std::map<
        std::string,
        std::unique_ptr<SalaPife>
    > salas_;

    static std::string gerarIdSala();

public:
    static SalaPife* criarSala(int maxJogadores);

    static SalaPife* obterSala(const std::string& idSala);

    static bool salaExiste(const std::string& idSala);

    static bool removerSala(const std::string& idSala);
};

#endif