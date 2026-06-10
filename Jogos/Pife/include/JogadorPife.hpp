#ifndef JOGADOR_PIFE_HPP
#define JOGADOR_PIFE_HPP

#include "Carta.hpp"

#include <string>
#include <vector>

class JogadorPife {
private:
    std::string nome_;
    std::vector<Carta> mao_;
    bool indiceValido(int indice) const;

public:
    JogadorPife();

    void definirNome(const std::string& nome);
    const std::string& nome() const;

    void receberCarta(const Carta& carta);
    void organizarMao();
    int tmnhMao() const;

    Carta descartarCarta(int indice);
    const std::vector<Carta>& verMao() const;
};

#endif