#ifndef PACIENCIA_H
#define PACIENCIA_H
#include <stack>
#include <vector>
#include "baralho.h"
using std::vector;
using std::stack;


enum class TipoPilha {
    Coluna,
    Descarte,
    Fundacao
};

class Paciencia
{
private:

    struct EstadoJogo {
        vector<std::vector<Carta>> colunas;
        vector<std::vector<Carta>> fundacoes;
        vector<Carta> descarte;
        Baralho cava;
    };
    vector<vector<Carta>> colunas; 
    Baralho cava;
    vector<Carta> descarte; //Cartas após clicar no Cava
    stack<EstadoJogo> historico;
/*  stack<Carta> pilhaCopas;
    stack<Carta> pilhaOuros;
    stack<Carta> pilhaEspadas;
    stack<Carta> pilhaPaus; */
    vector<vector<Carta>> fundacoes; //Pilha onde fica cada naipe 

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
    
    
    //void jogadasPossiveis(); Função caso sobre tempo!
    ~Paciencia();
};

#endif