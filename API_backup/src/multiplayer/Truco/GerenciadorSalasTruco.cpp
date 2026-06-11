#include "multiplayer/Truco/GerenciadorSalasTruco.hpp"
#include <random>

std::map<std::string, std::unique_ptr<SalaTruco>> GerenciadorSalasTruco::salas_;

std::string GerenciadorSalasTruco::gerarIdSala() {
    static const std::string caracteres = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    static std::random_device dispositivo;
    static std::mt19937 gerador(dispositivo());
    std::uniform_int_distribution<int> distribuicao(0, static_cast<int>(caracteres.size()) - 1);

    std::string id;

    do {
        id.clear();
        for (int i = 0; i < 6; ++i) id += caracteres[distribuicao(gerador)];
    } while (salaExiste(id));

    return id;
}

SalaTruco* GerenciadorSalasTruco::criarSala(TipoTruco tipo, int maxJogadores) {
    if (maxJogadores != 2 && maxJogadores != 4) return nullptr;

    std::string id = gerarIdSala();
    salas_[id] = std::make_unique<SalaTruco>(id, tipo, maxJogadores);

    return salas_[id].get();
}

SalaTruco* GerenciadorSalasTruco::obterSala(const std::string& idSala) {
    auto it = salas_.find(idSala);
    return it == salas_.end() ? nullptr : it->second.get();
}

bool GerenciadorSalasTruco::salaExiste(const std::string& idSala) {
    return salas_.find(idSala) != salas_.end();
}

bool GerenciadorSalasTruco::removerSala(const std::string& idSala) {
    return salas_.erase(idSala) > 0;
}