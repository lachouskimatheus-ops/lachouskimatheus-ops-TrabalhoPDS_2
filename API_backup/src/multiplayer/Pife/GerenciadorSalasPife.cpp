#include "multiplayer/Pife/GerenciadorSalasPife.hpp"

#include <random>

std::map<std::string, std::unique_ptr<SalaPife>> GerenciadorSalasPife::salas_;

std::string GerenciadorSalasPife::gerarIdSala() {
    static const std::string caracteres = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

    static std::random_device dispositivoAleatorio;
    static std::mt19937 gerador(dispositivoAleatorio());

    std::uniform_int_distribution<int> distribuicao(0, static_cast<int>(caracteres.size()) - 1);

    std::string idSala;

    do {
        idSala.clear();

        for (int i = 0; i < 6; i++) {
            idSala += caracteres[distribuicao(gerador)];
        }
    } while (salaExiste(idSala));

    return idSala;
}

SalaPife* GerenciadorSalasPife::criarSala(int maxJogadores) {
    if (maxJogadores < 2 || maxJogadores > 4) {
        return nullptr;
    }

    std::string idSala = gerarIdSala();

    salas_[idSala] = std::make_unique<SalaPife>(idSala, maxJogadores);

    return salas_[idSala].get();
}

SalaPife* GerenciadorSalasPife::obterSala(const std::string& idSala) {
    auto it = salas_.find(idSala);

    if (it == salas_.end()) {
        return nullptr;
    }

    return it->second.get();
}

bool GerenciadorSalasPife::salaExiste(const std::string& idSala) {
    return salas_.find(idSala) != salas_.end();
}

bool GerenciadorSalasPife::removerSala(const std::string& idSala) {
    auto it = salas_.find(idSala);

    if (it == salas_.end()) {
        return false;
    }

    salas_.erase(it);

    return true;
}