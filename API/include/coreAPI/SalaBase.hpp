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
    SalaBase(
        const std::string& idSala,
        int maxJogadores
    );

    virtual ~SalaBase() = default;

    int adicionarJogador(
        const std::string& tokenReconexao
    );

    int reconectarJogador(
        const std::string& tokenReconexao
    );

    bool desconectarJogador(int idJogador);

    int buscarJogadorPorToken(
        const std::string& tokenReconexao
    ) const;

    bool tokenExiste(
        const std::string& tokenReconexao
    ) const;

    bool jogadorEstaConectado(
        int idJogador
    ) const;

    bool podeAdicionarNovoJogador() const;

    std::string idSala() const;

    int maxJogadores() const;

    int jogadoresConectados() const;

    int jogadoresRegistrados() const;

    bool estaLotada() const;

    bool todosConectados() const;

    bool estaVazia() const;

    const std::vector<JogadorConectado>&
    jogadores() const;
};

#endif