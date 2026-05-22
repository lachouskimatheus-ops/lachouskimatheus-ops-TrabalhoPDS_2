#include "regras.h"
#include "paciencia.h"
#include <iostream>
#include <algorithm>
using std::cout;
using std::vector; // Garantindo o escopo do vector se não estiver no header

Paciencia::Paciencia() : vitoria(false), pontuacao() {
    iniciarJogo();
}

Paciencia::~Paciencia() {}

void Paciencia::iniciarJogo() {
    // Limpar estados anteriores
    colunas.assign(7, vector<Carta>());
    fundacoes.assign(4, vector<Carta>());
    descarte.clear();
    while(!historico.empty()) historico.pop();
    
    cava = Baralho();
    cava.embaralhar();
    
    // Distribuir cartas nas colunas 
    for (int i = 0; i < 7; ++i) {
        colunas[i].clear();
        cartasEscondidas[i] = i;
        for (int j = 0; j <= i; ++j) {
            Carta c = cava.retirarCarta();
            colunas[i].push_back(c);
        }
    }
    
    pontuacao.resetar();
    vitoria = false;
}

void Paciencia::virarParaCima(int colunaIndice) {
    if (cartasEscondidas[colunaIndice] > 0 && colunas[colunaIndice].size() > 0) {
        cartasEscondidas[colunaIndice]--;
        pontuacao.aplicar(EventoPontuacao::VirarCarta);
    }
}

bool Paciencia::cartaVisivel(int coluna, int linha) const {
    return (linha >= cartasEscondidas[coluna]);
}

void Paciencia::salvarJogada() {
    EstadoJogo estado;
    estado.colunas = colunas;
    estado.fundacoes = fundacoes;
    estado.descarte = descarte;
    estado.cava = cava;
    estado.pontos = pontuacao.getPontos();
    estado.passadasCava = pontuacao.getPassadasCava();
    for (int i = 0; i < 7; i++) estado.cartasEscondidas[i] = cartasEscondidas[i];

    historico.push(estado);
}

bool Paciencia::desfazer() {
    if (historico.empty()) return false;
    
    EstadoJogo anterior = historico.top();
    historico.pop();
    
    colunas = anterior.colunas;
    fundacoes = anterior.fundacoes;
    descarte = anterior.descarte;
    cava = anterior.cava;
    pontuacao.setPontos(anterior.pontos);
    pontuacao.setPassadasCava(anterior.passadasCava);
    for (int i = 0; i < 7; i++) cartasEscondidas[i] = anterior.cartasEscondidas[i];

    return true;
}

void Paciencia::comprarCarta() {
    salvarJogada();
    
    if (cava.estaVazio()) {
        if (descarte.empty()) return;
        
        std::reverse(descarte.begin(), descarte.end());
        for(const auto& c : descarte) {
            cava.inserirCarta(c);
        }
        descarte.clear();

        pontuacao.setPassadasCava(pontuacao.getPassadasCava() + 1);
        pontuacao.aplicar(EventoPontuacao::PassarBaralho);
    } else {
        Carta c = cava.retirarCarta();
        descarte.push_back(c);
    }
}

bool Paciencia::mover(TipoPilha origemTipo, int origemIndice, TipoPilha destinoTipo, int destinoIndice) {
    Carta* cartaParaMover = nullptr;
    vector<Carta>* pilhaOrigem = nullptr;

    if (origemTipo == TipoPilha::Descarte) {
        if (descarte.empty()) return false;
        cartaParaMover = &descarte.back();
        pilhaOrigem = &descarte;
    } else if (origemTipo == TipoPilha::Coluna) {
        if (colunas[origemIndice].empty()) return false;
        cartaParaMover = &colunas[origemIndice].back();
        pilhaOrigem = &colunas[origemIndice];
    }

    if (!cartaParaMover) return false;

    bool movimentoValido = false;
    EventoPontuacao evento = EventoPontuacao::CavaParaColuna;

    if (destinoTipo == TipoPilha::Fundacao) {
        if (Regras::podeMoverParaFundacao(*cartaParaMover, fundacoes[destinoIndice])) {
            movimentoValido = true;
            evento = (origemTipo == TipoPilha::Descarte) ? EventoPontuacao::CavaParaFundacao : EventoPontuacao::ColunaParaFundacao;
        }
    } else if (destinoTipo == TipoPilha::Coluna) {
        if (colunas[destinoIndice].empty()) {
            if (Regras::podeMoverParaColunaVazia(*cartaParaMover)) movimentoValido = true;
        } else {
            if (Regras::podeMoverParaColuna(*cartaParaMover, colunas[destinoIndice].back())) movimentoValido = true;
        }
        evento = EventoPontuacao::CavaParaColuna; 
    }

    if (movimentoValido) {
        salvarJogada();
        
        Carta c = *cartaParaMover;
        pilhaOrigem->pop_back();
        
        if (destinoTipo == TipoPilha::Fundacao) {
            fundacoes[destinoIndice].push_back(c);
        } else {
            colunas[destinoIndice].push_back(c);
        }

        if (origemTipo == TipoPilha::Coluna && !colunas[origemIndice].empty()) {
           int tamanho = (int)colunas[origemIndice].size();
           if (tamanho == cartasEscondidas[origemIndice] && cartasEscondidas[origemIndice] > 0) {
               virarParaCima(origemIndice);
           }
        }

        pontuacao.aplicar(evento);
        verificarVitoria();
        return true;
    }

    return false;
}

bool Paciencia::verificarVitoria() {
    for (const auto& f : fundacoes) {
        if (f.size() != 13) return false;
    }
    vitoria = true;
    pontuacao.salvarRecord();
    return true;
}

bool Paciencia::existeJogadaPossivel() {
    // 1. Verifica se há carta no descarte que pode ir para algum lugar
    if (!descarte.empty()) {
        const Carta& topo = descarte.back();
        for (int i = 0; i < 4; i++) {
            if (Regras::podeMoverParaFundacao(topo, fundacoes[i])) return true;
        }
        for (int i = 0; i < 7; i++) {
            if (colunas[i].empty()) {
                if (Regras::podeMoverParaColunaVazia(topo)) return true;
            } else {
                if (Regras::podeMoverParaColuna(topo, colunas[i].back())) return true;
            }
        }
    }

    // 2. Verifica se há carta no topo das colunas que pode mover (CORRIGIDO: Removido o ';' do for)
    for (int i = 0; i < 7; i++) {
        if (colunas[i].empty()) continue;
        const Carta& topo = colunas[i].back();
        for (int k = 0; k < 4; k++) {
            if (Regras::podeMoverParaFundacao(topo, fundacoes[k])) return true;
        }
        for (int j = 0; j < 7; j++) {
            if (i == j) continue;
            if (colunas[j].empty()) {
                if (Regras::podeMoverParaColunaVazia(topo)) return true;
            } else {
                if (Regras::podeMoverParaColuna(topo, colunas[j].back())) return true;
            }
        }
    }

    // 3. Se ainda houver cartas para comprar, o jogo não travou
    if (!cava.estaVazio() || !descarte.empty()) {
        return true;
    }

    return false;
}

int Paciencia::getPontuacao() const {
    return pontuacao.getPontos();
}

void Paciencia::imprimirJogo() {
    std::cout << "\n========================================\n";
    std::cout << "PONTOS: " << pontuacao.getPontos() << " | RECORDE: " << pontuacao.getRecord() << "\n";
    std::cout << "========================================\n";
    
    // Cava e Descarte (CORRIGIDO: Unificado dentro da função)
    cout << "Cava [" << cava.tamanho() << "] | Descarte: ";
    if (descarte.empty()) {
        std::cout << "[ vazio ]";
    } else {
        cout << "[" << descarte.back().cartaString() << "]";
    }
    cout << "\n\n";
 
    // Fundações
    cout << "Fundacoes: ";
    for (int i = 0; i < 4; i++) {
        if (fundacoes[i].empty()) {
            cout << "[ _ ] ";
        } else {
            cout << "[" << fundacoes[i].back().cartaString() << "] ";
        }
    }
    cout << "\n\n";
 
    // Colunas
    cout << "Colunas:\n";
    int maxLinhas = 0;
    for (int i = 0; i < 7; i++) {
        if ((int)colunas[i].size() > maxLinhas) maxLinhas = (int)colunas[i].size();
    }
 
    // Cabeçalho das colunas
    for (int i = 0; i < 7; i++) {
        cout << "  Col" << (i + 1) << "  \t";
    }
    cout << "\n";
 
    for (int linha = 0; linha < maxLinhas; linha++) {
        for (int col = 0; col < 7; col++) {
            if (linha < (int)colunas[col].size()) {
                // CORRIGIDO: Modificado de 'estaExposta' para '!cartaVisivel'
                if (!cartaVisivel(col, linha)) {
                    cout << "[   ???   ]\t";
                } else {
                    cout << "[" << colunas[col][linha].cartaString() << "]\t";
                }
            } else {
                cout << "        \t";
            }
        }
        cout << "\n";
    }
    cout << "\n";
 
    if (vitoria) {
        cout << "*** PARABENS! VOCE GANHOU! ***\n";
    }
}