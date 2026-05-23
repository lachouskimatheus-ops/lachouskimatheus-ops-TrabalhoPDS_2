#ifndef PACIENCIA_H
#define PACIENCIA_H

#include <stack>
#include <vector>
#include "baralho.h"
#include "pontuacao.h"
#include <map>

using std::vector;
using std::stack;

enum class TipoPilha {
    Coluna,
    Descarte,
    Fundacao
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
    bool desfazer();
    void salvarJogada();
    void iniciarJogo();
    void imprimirJogo();
    void comprarCarta();
    int getPontuacao() const;
    void virarParaCima(int coluna);
    bool estaExposta(int coluna, int linha);
    ~Paciencia();
};

#endif