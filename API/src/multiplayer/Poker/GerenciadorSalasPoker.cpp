#include "multiplayer/Poker/GerenciadorSalasPoker.hpp"
#include <random>

std::map<std::string, std::unique_ptr<SalaPoker>> GerenciadorSalasPoker::salas_;

std::string GerenciadorSalasPoker::gerarIdSala() {
    static const std::string caracteres = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    static std::random_device dispositivo;
    static std::mt19937 gerador(dispositivo());
    static std::uniform_int_distribution<int> distribuicao(0, static_cast<int>(caracteres.size()) - 1);

    std::string id;
    do {
        id.clear();
        for (int i = 0; i < 6; i++) id += caracteres[distribuicao(gerador)];
    } while (salaExiste(id));

    return id;
}

SalaPoker* GerenciadorSalasPoker::criarSala(int maxJogadores, ModoPoker modo) {
    if (modo == ModoPoker::ContraComputador) maxJogadores = 1;
    else if (maxJogadores < 2 || maxJogadores > 4) return nullptr;

    std::string idSala = gerarIdSala();
    salas_[idSala] = std::make_unique<SalaPoker>(idSala, maxJogadores, modo);
    return salas_[idSala].get();
}

SalaPoker* GerenciadorSalasPoker::obterSala(const std::string& idSala) {
    auto it = salas_.find(idSala);
    return it == salas_.end() ? nullptr : it->second.get();
}

bool GerenciadorSalasPoker::salaExiste(const std::string& idSala) {
    return salas_.find(idSala) != salas_.end();
}

bool GerenciadorSalasPoker::removerSala(const std::string& idSala) {
    return salas_.erase(idSala) > 0;
}

bool GerenciadorSalasPoker::removerSalaSeVazia(const std::string& idSala) {
    SalaPoker* sala = obterSala(idSala);
    return sala && sala->estaVazia() && removerSala(idSala);
}