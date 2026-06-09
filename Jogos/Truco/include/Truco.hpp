#pragma once

#include <string>
#include <vector>
#include "Baralho.hpp"
#include "Carta.hpp"
#include "Jogador_Truco.hpp"
#include "JuizTruco.hpp"

enum class FaseTruco {
    AguardandoInicio,
    AguardandoJogada,
    AguardandoRespostaTruco,
    MaoFinalizada,
    PartidaFinalizada
};

struct JogadaTruco {
    int idJogador;
    Carta* carta;
};

class Truco {
private:
    std::vector<Jogador_Truco*> jogadores_;
    std::vector<int> equipes_;
    std::vector<JogadaTruco> jogadasDaQueda_;
    std::vector<Carta*> cartasDaMao_;

    Carta* vira_;
    Baralho* baralho_;
    JuizTruco* juiz_;
    FaseTruco fase_;

    int pontosEquipe1_;
    int pontosEquipe2_;
    int valorMao_;
    int jogadorAtual_;
    int jogadorInicialMao_;
    int jogadorInicialQueda_;
    int numeroQueda_;
    int vitoriasEquipe1_;
    int vitoriasEquipe2_;
    int vencedorPrimeiraQueda_;
    int vencedorUltimaQueda_;
    int vencedorMao_;
    int vencedorPartida_;
    int equipeQuePediu_;
    int valorPedido_;

    bool maoTravada_;
    std::string nomePedidor_;
    std::string ultimoEvento_;
    std::string ultimaMensagem_;

    bool jogadorValido(int idJogador) const;
    bool equipeValida(int equipe) const;
    bool equipesCompletas() const;
    int proximoValorMao(int valorAtual) const;
    int resolverVencedorDaMao(int vencedorQueda) const;

    void limparCartasDaMao();
    void distribuirCartas();
    void avancarJogador();
    void concluirQueda();
    void iniciarProximaQueda(int jogadorInicial);
    void finalizarMao(int equipeVencedora, int pontosGanhos);

public:
    Truco(JuizTruco* juiz, Baralho* baralho);
    ~Truco();

    Truco(const Truco&) = delete;
    Truco& operator=(const Truco&) = delete;

    bool adicionarJogador(Jogador_Truco* jogador, int equipe);
    bool iniciarPartida();
    bool iniciarNovaMao();
    bool jogarCarta(int idJogador, int indiceCarta);
    bool pedirTruco(int idJogador);
    bool aceitarTruco(int idJogador);
    bool recusarTruco(int idJogador);
    bool aumentarTruco(int idJogador);

    bool podeJogar(int idJogador) const;
    bool podePedirTruco(int idJogador) const;
    bool podeResponderTruco(int idJogador) const;
    bool equipeDisponivel(int equipe, int maxJogadores) const;

    int getEquipeDoJogador(int idJogador) const;
    FaseTruco getFase() const;
    int getJogadorAtual() const;
    int getNumeroQueda() const;
    int getValorMao() const;
    int getPontosEquipe1() const;
    int getPontosEquipe2() const;
    int getVencedorUltimaQueda() const;
    int getVencedorMao() const;
    int getVencedorPartida() const;
    int getEquipeQuePediu() const;
    int getValorPedido() const;
    int getQuantidadeJogadores() const;
    int getQuantidadeNaEquipe(int equipe) const;
    int getProximoValorMao() const;
    int getJogadorInicialQueda() const;
    int getVencedorPrimeiraQueda() const;
    int getVitoriasEquipe1() const;
    int getVitoriasEquipe2() const;

    bool getMaoTravada() const;
    const std::string& getNomePedidor() const;
    const std::string& getUltimoEvento() const;
    const std::string& getUltimaMensagem() const;
    const Carta* getVira() const;
    const std::vector<JogadaTruco>& getJogadasDaQueda() const;
    const std::vector<Jogador_Truco*>& getJogadores() const;
};