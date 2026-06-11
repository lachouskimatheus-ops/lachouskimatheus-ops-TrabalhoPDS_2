#ifndef EXCECOES_PIFE_HPP
#define EXCECOES_PIFE_HPP

#include <stdexcept>
#include <string>

class ErroPife : public std::runtime_error {
public:
    explicit ErroPife(const std::string& mensagem)
        : std::runtime_error(mensagem) {}
};

class QuantidadeJogadoresInvalida : public ErroPife {
public:
    QuantidadeJogadoresInvalida()
        : ErroPife("A quantidade de jogadores deve estar entre 2 e 4.") {}
};

class JogadorInvalido : public ErroPife {
public:
    JogadorInvalido()
        : ErroPife("O jogador informado não existe.") {}
};

class JogoFinalizado : public ErroPife {
public:
    JogoFinalizado()
        : ErroPife("A partida já foi finalizada.") {}
};

class ForaDoTurno : public ErroPife {
public:
    ForaDoTurno()
        : ErroPife("Não é o turno deste jogador.") {}
};

class FaseTurnoInvalida : public ErroPife {
public:
    explicit FaseTurnoInvalida(const std::string& mensagem)
        : ErroPife(mensagem) {}
};

class MesaVazia : public ErroPife {
public:
    MesaVazia()
        : ErroPife("Não há cartas disponíveis na mesa.") {}
};

class BaralhoIndisponivel : public ErroPife {
public:
    BaralhoIndisponivel()
        : ErroPife("Não há cartas disponíveis no baralho.") {}
};

class IndiceCartaInvalido : public ErroPife {
public:
    IndiceCartaInvalido()
        : ErroPife("O índice da carta é inválido.") {}
};

class CartaInvalida : public ErroPife {
public:
    CartaInvalida()
        : ErroPife("A carta informada é inválida.") {}
};

class MaoInvalida : public ErroPife {
public:
    MaoInvalida()
        : ErroPife("A mão não possui combinações válidas para bater.") {}
};

#endif