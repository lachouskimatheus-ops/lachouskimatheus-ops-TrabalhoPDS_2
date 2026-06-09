#ifndef PACIENCIA_H
#define PACIENCIA_H

#include <vector>
#include <stack>
#include <string>
#include <map>
#include <set>
#include "Baralho.hpp"
#include "Pontuacao.hpp"

// Enumerações fora da classe para melhor reuso
enum class TipoPilha {
    Coluna,
    Descarte,
    Fundacao
};

// Struct para representar a assinatura de uma jogada
struct JogadaSimulada {
    std::string tipoAcao;
    TipoPilha origemTipo;
    int origemIdx;
    TipoPilha destinoTipo;
    int destinoIdx;
    int cartaIdx; 
};

class Paciencia {
private:
    // Estado interno 
    struct EstadoJogo {
        std::vector<std::vector<Carta>> colunas;
        std::vector<std::vector<Carta>> fundacoes;
        std::vector<Carta> descarte;
        Baralho cava;
        int pontos;
        int passadasCava;
        int cartasEscondidas[7];
    };

    // Membros da classe
    bool vitoria;
    std::vector<std::vector<Carta>> colunas;
    std::vector<std::vector<Carta>> fundacoes;
    std::vector<Carta> descarte;
    Baralho cava;
    int cartasEscondidas[7];
    
    std::stack<EstadoJogo> historico;
    Pontuacao pontuacao;

    // Métodos utilitários privados
    void salvarEstadoNoHistorico();
    void restaurarEstadoDoHistorico();

    // Métodos auxiliares para organizar a listagem de jogadas
    void coletarJogadasDescarte(std::vector<JogadaSimulada>& jogadas) const;
    void coletarJogadasColunas(std::vector<JogadaSimulada>& jogadas) const;
    void coletarJogadasFundacao(std::vector<JogadaSimulada>& jogadas) const;

  private:
    // Auxiliares de validação (retornam true se o movimento é permitido)
    bool podeMover(TipoPilha origemTipo, int origemIdx, TipoPilha destinoTipo, int destinoIdx) const;
    bool podeMoverBloco(int origemCol, int cartaIdx, int destinoCol) const;
    bool podeMoverDaFundacao(int fundacaoIdx, int destinoCol) const;

    // Auxiliares de execução (realizam a alteração do estado)
    void executarMovimento(TipoPilha origemTipo, int origemIdx, TipoPilha destinoTipo, int destinoIdx);
    void executarMovimentoBloco(int origemCol, int cartaIdx, int destinoCol);
    void executarMovimentoDaFundacao(int fundacaoIdx, int destinoCol);

    // Auxiliar para pontuação
    EventoPontuacao definirEvento(TipoPilha origem, TipoPilha destino) const;

public:
    Paciencia();
    ~Paciencia();

    // --- Ciclo de Vida do Jogo ---
    void iniciarJogo();
    void gerarJogoReversivel();
    void comprarCarta();
    bool desfazer();
    
    // --- Comandos de Movimento ---
    bool mover(TipoPilha origemTipo, int origemIndice, TipoPilha destinoTipo, int destinoIndice);
    bool moverBloco(int origemColuna, int cartaIdx, int destinoColuna);
    bool moverDaFundacao(int fundacaoIndice, TipoPilha destinoTipo, int destinoIndice);
    bool moverUmaParaFundacao();
    void virarParaCima(int coluna);

    // --- Verificações e IA ---
    bool existeJogadaPossivel() const;
    bool verificarVitoria();
    bool cartaVisivel(int colunaIdx, int cartaIdx) const;
    bool estaExposta(int coluna, int linha) const;
    void completarAutomaticamente();
    
    // IA / Solver
    std::vector<JogadaSimulada> listarJogadasPossiveis();
    bool simularSolucao(std::set<std::string>& estadosVisitados);
    bool garantirJogoVencivel();

    // --- Getters (Interface para o Servidor) ---
    const std::vector<std::vector<Carta>>& getColunas() const { return colunas; }
    const std::vector<std::vector<Carta>>& getFundacoes() const { return fundacoes; }
    const std::vector<Carta>& getDescarte() const { return descarte; }
    int getCavaTamanho() const { return cava.tamanho(); }
    int getCartasEscondidas(int i) const { return (i >= 0 && i < 7) ? cartasEscondidas[i] : 0; }
    int getPontuacao() const;
    bool getVitoria() const { return vitoria; }
    
    // --- Debug ---
    void imprimirJogo();
    std::string converterParaString() const;
};

#endif