#include "multiplayer/Pife/GerenciadorSalasPife.hpp"

SalaPife* GerenciadorSalasPife::obterOuCriarSala(
    const std::string& idSala,
    int maxJogadores
) {
    auto it = salas_.find(idSala);

    if (it != salas_.end()) {
        return it->second.get();
    }

    salas_[idSala] = std::make_unique<SalaPife>(
        idSala,
        maxJogadores
    );

    return salas_[idSala].get();
}