#include "regras.h"
#include "paciencia.h"
#include <iostream>
#include <algorithm>

Paciencia::Paciencia() : vitoria(false), pontuacao() {
    iniciarJogo();
}

Paciencia::~Paciencia() {}

void Paciencia::iniciarJogo() {
    // 1. Limpar estados anteriores
    colunas.assign(7, vector<Carta>());
    fundacoes.assign(4, vector<Carta>());
    descarte.clear();
    while(!historico.empty()) historico.pop();
    
    // 2. Preparar baralho
    cava = Baralho();
    cava.embaralhar();
    
    // 3. Distribuir cartas nas colunas (estilo clássico)
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
        // Se a coluna não está vazia e ainda havia cartas escondidas,
        // diminuímos o contador de escondidas para mostra a nova última carta.
        cartasEscondidas[colunaIndice]--;
        pontuacao.aplicar(EventoPontuacao::VirarCarta);
    }
}

bool Paciencia::estaExposta(int coluna, int linha) {
    // Se a posição da carta for maior ou igual ao número de escondidas,
    // significa que ela está virada para cima.
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
    
    return true;
}

void Paciencia::comprarCarta() {
    salvarJogada();
    
    if (cava.estaVazio()) {
        // Reinserir descarte na cava (viradas para baixo)
        if (descarte.empty()) return;
        
        // Inverter o descarte para voltar à cava
        std::reverse(descarte.begin(), descarte.end());
        for(auto& c : descarte) {
            c.virarParaBaixo();
            // Aqui você precisaria de um método no Baralho para adicionar cartas, 
            // ou manipular o vector 'cartas' se fosse visível.
        }
        // Nota: Se sua classe Baralho não permite reinserir, 
        // o ideal é tratar a cava como um vector aqui também.
        
        pontuacao.setPassadasCava(pontuacao.getPassadasCava() + 1);
        pontuacao.aplicar(EventoPontuacao::PassarBaralho);
        descarte.clear();
    } else {
        Carta c = cava.retirarCarta();
        c.virarParaCima();
        descarte.push_back(c);
    }
}

bool Paciencia::mover(TipoPilha origemTipo, int origemIndice, TipoPilha destinoTipo, int destinoIndice) {
    Carta* cartaParaMover = nullptr;
    vector<Carta>* pilhaOrigem = nullptr;

    // 1. Identificar a carta de origem
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

    // 2. Validar o movimento com a classe Regras
    bool movimentoValido = false;
    EventoPontuacao evento;

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
        evento = EventoPontuacao::CavaParaColuna; // Simplificado
    }

    // 3. Executar o movimento se for válido
    if (movimentoValido) {
        salvarJogada();
        
        Carta c = *cartaParaMover;
        pilhaOrigem->pop_back();
        
        if (destinoTipo == TipoPilha::Fundacao) {
            fundacoes[destinoIndice].push_back(c);
        }
        else {
            colunas[destinoIndice].push_back(c);
        }
        // Se revelar uma carta na coluna de origem, ganha pontos
        if (origemTipo == TipoPilha::Coluna && !colunas[origemIndice].empty()) {
            if (colunas[origemIndice].size() ==  (size_t)cartasEscondidas[origemIndice] && cartasEscondidas[origemIndice] > 0) { // Assumindo método isFaceUp
                virarParaCima();
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

int Paciencia::getPontuacao() const {
    return pontuacao.getPontos();
}

void Paciencia::imprimirJogo() {
    std::cout << "\n========================================\n";
    std::cout << "PONTOS: " << pontuacao.getPontos() << " | RECORDE: " << pontuacao.getRecord() << "\n";
    std::cout << "========================================\n";
    std::cout << "Cava: " << cava.tamanho() << " cartas | Descarte: " << (descarte.empty() ? "[ ]" : "Última carta") << "\n";
}