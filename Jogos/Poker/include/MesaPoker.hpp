#pragma once

#include <string>
#include <vector>
#include "Baralho.hpp"
#include "Poker.hpp"

enum class FasePoker {
    AguardandoJogadores,
    EscolhendoTrocas,
    Resultado
};

enum class ModoPoker {
    ContraComputador,
    Multiplayer
};

class MesaPoker {
private:
    Baralho baralho_;
    std::vector<Poker> jogadores_;
    std::vector<std::string> nomes_;
    std::vector<int> pontos_;
    std::vector<bool> confirmouTroca_;
    std::vector<std::vector<int>> trocasPendentes_;
    std::vector<int> quantidadeUltimaTroca_;

    int quantidadeJogadores_;
    int rodada_;
    int vencedorRodada_;
    int empates_;

    ModoPoker modo_;
    FasePoker fase_;

    void distribuirCartas();
    void limparConfirmacoes();
    void realizarJogadaComputador();
    void executarTrocas();
    void finalizarRodada();

    std::vector<int> escolherTrocasComputador() const;
    bool indicesTrocaValidos(const std::vector<int>& indices) const;
    bool todosConfirmaram() const;

public:
    MesaPoker(int quantidadeJogadores, ModoPoker modo);

    void iniciar();
    bool iniciarNovaRodada();

    void definirNomeJogador(int idJogador, const std::string& nome);

    bool confirmarTroca(int idJogador, const std::vector<int>& indices);
    bool podeConfirmarTroca(int idJogador) const;
    bool jogadorValido(int idJogador) const;

    const Poker& jogador(int idJogador) const;
    const std::vector<int>& pontos() const;
    const std::vector<int>& quantidadeUltimaTroca() const;
    const std::string& nomeJogador(int idJogador) const;

    bool jogadorConfirmouTroca(int idJogador) const;

    int quantidadeJogadores() const;
    int rodada() const;
    int vencedorRodada() const;
    int empates() const;

    ModoPoker modo() const;
    FasePoker fase() const;
};