#ifndef GERENCIADOR_SALAS_PIFE_HPP
#define GERENCIADOR_SALAS_PIFE_HPP

#include <map>
#include <memory>
#include <string>

#include "multiplayer/Pife/SalaPife.hpp"

class GerenciadorSalasPife {
private:
    std::map<std::string, std::unique_ptr<SalaPife>> salas_;

public:
    SalaPife* obterOuCriarSala(const std::string& idSala, int maxJogadores);
};

#endif