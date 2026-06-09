#include "multiplayer/FDP/GerenciadorSalasFDP.hpp"

#include <random>

std::map<
    std::string,
    std::shared_ptr<SalaFDP>
> GerenciadorSalasFDP::salas_;

std::mutex GerenciadorSalasFDP::mutexSalas_;

std::string GerenciadorSalasFDP::gerarIdSala() {
    static const std::string caracteres =
        "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

    static std::random_device dispositivoAleatorio;

    static std::mt19937 gerador(
        dispositivoAleatorio()
    );

    std::uniform_int_distribution<int> distribuicao(
        0,
        static_cast<int>(caracteres.size()) - 1
    );

    std::string idSala;

    do {
        idSala.clear();

        for (int i = 0; i < 6; i++) {
            idSala += caracteres[
                distribuicao(gerador)
            ];
        }
    } while (salas_.find(idSala) != salas_.end());

    return idSala;
}

std::shared_ptr<SalaFDP>
GerenciadorSalasFDP::criarSala(
    int maxJogadores
) {
    if (
        maxJogadores < 2
        || maxJogadores > 4
    ) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(
        mutexSalas_
    );

    std::string idSala = gerarIdSala();

    std::shared_ptr<SalaFDP> sala =
        std::make_shared<SalaFDP>(
            idSala,
            maxJogadores
        );

    salas_[idSala] = sala;

    return sala;
}

std::shared_ptr<SalaFDP>
GerenciadorSalasFDP::obterSala(
    const std::string& idSala
) {
    std::lock_guard<std::mutex> lock(
        mutexSalas_
    );

    auto it = salas_.find(idSala);

    if (it == salas_.end()) {
        return nullptr;
    }

    return it->second;
}

bool GerenciadorSalasFDP::salaExiste(
    const std::string& idSala
) {
    std::lock_guard<std::mutex> lock(
        mutexSalas_
    );

    return salas_.find(idSala)
        != salas_.end();
}

bool GerenciadorSalasFDP::removerSala(
    const std::string& idSala
) {
    std::lock_guard<std::mutex> lock(
        mutexSalas_
    );

    auto it = salas_.find(idSala);

    if (it == salas_.end()) {
        return false;
    }

    salas_.erase(it);

    return true;
}

bool GerenciadorSalasFDP::removerSalaSeVazia(
    const std::string& idSala
) {
    std::lock_guard<std::mutex> lock(
        mutexSalas_
    );

    auto it = salas_.find(idSala);

    if (it == salas_.end()) {
        return false;
    }

    if (!it->second->estaVazia()) {
        return false;
    }

    salas_.erase(it);

    return true;
}

std::vector<std::string>
GerenciadorSalasFDP::listarIdsSalas() {
    std::lock_guard<std::mutex> lock(
        mutexSalas_
    );

    std::vector<std::string> ids;

    ids.reserve(salas_.size());

    for (const auto& par : salas_) {
        ids.push_back(par.first);
    }

    return ids;
}
