#ifndef SALA_BASE_HPP
#define SALA_BASE_HPP

#include <string>
#include <vector>

#include "coreAPI/JogadorConectado.hpp"

class SalaBase {
protected:
    std::string idSala_;
    int maxJogadores_;
    int jogadoresConectados_;
    std::vector<JogadorConectado> jogadores_;

public:
    SalaBase(const std::string& idSala, int maxJogadores);

    virtual ~SalaBase() = default;

    virtual int adicionarJogador();

    virtual bool removerJogador(int idJogador);

    std::string idSala() const;

    int maxJogadores() const;
    int jogadoresConectados() const;

    bool estaCheia() const;
    bool estaVazia() const;

    const std::vector<JogadorConectado>&
    jogadores() const;
};

#endif