#pragma once

#include <map>
#include <memory>
#include <string>

#include "multiplayer/BlackJack/SalaBlackJack.hpp"

/**
 * @brief Controla todas as salas ativas de Blackjack/21.
 */
class GerenciadorSalasBlackJack {
private:
    static std::map<std::string, std::unique_ptr<SalaBlackJack>> salas_;

    static std::string gerarIdSala();

public:
    static SalaBlackJack* criarSala(int maxJogadores);

    static SalaBlackJack* obterSala(const std::string& idSala);

    static bool salaExiste(const std::string& idSala);

    static bool removerSala(const std::string& idSala);

    static bool removerSalaSeVazia(const std::string& idSala);
};
