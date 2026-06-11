#include "multiplayer/BlackJack/GerenciadorSalasBlackJack.hpp"

#include <random>

std::map<std::string, std::unique_ptr<SalaBlackJack>>
GerenciadorSalasBlackJack::salas_;

std::string GerenciadorSalasBlackJack::gerarIdSala() {
    static const std::string caracteres = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    static std::random_device dispositivo;
    static std::mt19937 gerador(dispositivo());
    static std::uniform_int_distribution<int> distribuicao(
        0,
        static_cast<int>(caracteres.size()) - 1
    );

    std::string id;

    do {
        id.clear();

        for (int i = 0; i < 6; i++) {
            id += caracteres[distribuicao(gerador)];
        }
    } while (salaExiste(id));

    return id;
}

SalaBlackJack* GerenciadorSalasBlackJack::criarSala(int maxJogadores) {
    if (maxJogadores < 1 || maxJogadores > 4) {
        return nullptr;
    }

    std::string idSala = gerarIdSala();

    salas_[idSala] =
        std::make_unique<SalaBlackJack>(idSala, maxJogadores);

    return salas_[idSala].get();
}

SalaBlackJack* GerenciadorSalasBlackJack::obterSala(const std::string& idSala) {
    auto it = salas_.find(idSala);

    if (it == salas_.end()) {
        return nullptr;
    }

    return it->second.get();
}

bool GerenciadorSalasBlackJack::salaExiste(const std::string& idSala) {
    return salas_.find(idSala) != salas_.end();
}

bool GerenciadorSalasBlackJack::removerSala(const std::string& idSala) {
    return salas_.erase(idSala) > 0;
}

bool GerenciadorSalasBlackJack::removerSalaSeVazia(const std::string& idSala) {
    SalaBlackJack* sala = obterSala(idSala);

    if (sala == nullptr) {
        return false;
    }

    if (!sala->estaVazia()) {
        return false;
    }

    return removerSala(idSala);
}
