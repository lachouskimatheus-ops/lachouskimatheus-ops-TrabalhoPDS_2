#ifndef PACIENCIA_H
#define PACIENCIA_H

#include <stack>
#include <vector>
#include "Baralho.hpp"
#include "pontuacao.h"
#include <map>
#include <set>

using std::vector;
using std::stack;




enum class TipoPilha {
    Coluna,
    Descarte,
    Fundacao
};


struct JogadaSimulada {
    std::string tipoAcao; // "MOVER", "MOVER_BLOCO", "MOVER_DA_FUNDACAO", "COMPRAR"
    
    // Dados para MOVER normal ou MOVER_DA_FUNDACAO
    TipoPilha origemTipo;
    int origemIdx;
    TipoPilha destinoTipo;
    int destinoIdx;
    
    // Dados extras para MOVER_BLOCO
    int cartaIdx; 
};

class Paciencia {
private:
    struct EstadoJogo {
        vector<vector<Carta>> colunas;
        vector<vector<Carta>> fundacoes;
        vector<Carta> descarte;
        Baralho cava;
        int pontos;        // snapshot da pontuação
        int passadasCava;  // snapshot das passadas
        int cartasEscondidas[7];
    };
    
    int cartasEscondidas[7];
    bool vitoria;
    vector<vector<Carta>> colunas;
    Baralho cava;
    vector<Carta> descarte;
    vector<vector<Carta>> fundacoes;
    stack<EstadoJogo> historico;
    Pontuacao pontuacao; 

public:
    Paciencia();
    bool existeJogadaPossivel();
    bool verificarVitoria();
    bool cartaVisivel(int colunaIdx, int cartaIdx) const;
    bool mover(TipoPilha origemTipo, int origemIndice, TipoPilha destinoTipo, int destinoIndice);
    bool moverBloco(int origemColuna, int cartaIdx, int destinoColuna);
    bool moverDaFundacao(int fundacaoIndice, TipoPilha destinoTipo, int destinoIndice);
    bool desfazer();
    void salvarJogada();
    void iniciarJogo();
    void imprimirJogo();
    void comprarCarta();
    int getPontuacao() const;
    void virarParaCima(int coluna);
    bool estaExposta(int coluna, int linha);
    std::string converterParaString();   
    // Dentro de paciencia.h, na seção public:
std::vector<JogadaSimulada> listarJogadasPossiveis();
void completarAutomaticamente();
bool moverUmaParaFundacao();
bool simularSolucao(std::set<std::string>& estadosVisitados);
bool garantirJogoVencivel();   
void gerarJogoReversivel();
    // Getters para o servidor web
const std::vector<std::vector<Carta>>& getColunas() const { return colunas; }
const std::vector<std::vector<Carta>>& getFundacoes() const { return fundacoes; }
const std::vector<Carta>& getDescarte() const { return descarte; }
int getCavaTamanho() const { return cava.tamanho(); }
int getCartasEscondidas(int i) const { return cartasEscondidas[i]; }
bool getVitoria() const { return vitoria; }
    ~Paciencia();
};

#endif