#include "regras.h"
#include "paciencia.h"
#include <iostream>
#include <algorithm>
#include <random>
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
        // Cada fundação tem um naipe fixo
        // 0 = Ouro, 1 = Copa, 2 = Espada, 3 = Paus
        Naipe naipeFundacao;
        switch (destinoIndice) {
            case 0: naipeFundacao = Naipe::Paus;   break;
            case 1: naipeFundacao = Naipe::Copa;   break;
            case 2: naipeFundacao = Naipe::Espada; break;
            case 3: naipeFundacao = Naipe::Ouro;   break;
            default: return false;
        }
 
        // Carta deve ser do naipe correto da fundação
        if (cartaParaMover->mostraNaipe() != naipeFundacao) return false;
 
        if (Regras::podeMoverParaFundacao(*cartaParaMover, fundacoes[destinoIndice])) {
            movimentoValido = true;
            evento = (origemTipo == TipoPilha::Descarte) ? 
                      EventoPontuacao::CavaParaFundacao : 
                      EventoPontuacao::ColunaParaFundacao;
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


bool Paciencia::moverBloco(int origemColuna, int cartaIdx, int destinoColuna) {
    // 1. Validações básicas de limites dos índices
    if (origemColuna < 0 || origemColuna >= 7 || destinoColuna < 0 || destinoColuna >= 7) return false;
    if (origemColuna == destinoColuna) return false;

    auto& colOrigem = colunas[origemColuna];
    auto& colDestino = colunas[destinoColuna];

    if (colOrigem.empty()) return false;
    if (cartaIdx < 0 || cartaIdx >= (int)colOrigem.size()) return false;

    // 2. Verificar se a carta que inicia o bloco está visível (aberta)
    if (!cartaVisivel(origemColuna, cartaIdx)) return false;

    // 3. Validar a movimentação da base do bloco em relação ao destino
    const Carta& baseBloco = colOrigem[cartaIdx];

    bool movimentoValido = false;
    if (colDestino.empty()) {
        if (Regras::podeMoverParaColunaVazia(baseBloco)) {
            movimentoValido = true;
        }
    } else {
        if (Regras::podeMoverParaColuna(baseBloco, colDestino.back())) {
            movimentoValido = true;
        }
    }

    if (!movimentoValido) return false;

    // 4. Se for válido, salvar o estado atual para o "Desfazer"
    salvarJogada();

    // 5. Transferir o bloco mantendo a ordem original
    // Copiamos as cartas do índice selecionado até o final para a coluna de destino
    for (size_t i = cartaIdx; i < colOrigem.size(); ++i) {
        colDestino.push_back(colOrigem[i]);
    }

    // Removemos o bloco da coluna de origem
    colOrigem.erase(colOrigem.begin() + cartaIdx, colOrigem.end());

    // 6. Se a coluna de origem ficou com uma carta fechada no topo, revela ela
    if (!colOrigem.empty()) {
        int tamanho = (int)colOrigem.size();
        if (tamanho == cartasEscondidas[origemColuna] && cartasEscondidas[origemColuna] > 0) {
            virarParaCima(origemColuna);
        }
    }

    // Como as regras de pontuação oficiais costumam pontuar de forma similar a mover da coluna,
    // aplicamos o evento padrão de coluna.
    pontuacao.aplicar(EventoPontuacao::CavaParaColuna); 
    verificarVitoria();

    return true;
}

bool Paciencia::moverDaFundacao(int fundacaoIndice, TipoPilha destinoTipo, int destinoIndice) {
    // 1. Validações básicas e garantia de que o destino só pode ser uma Coluna
    if (fundacaoIndice < 0 || fundacaoIndice >= 4) return false;
    if (destinoTipo != TipoPilha::Coluna) return false; // Regra padrão: fundação só volta para coluna
    if (destinoIndice < 0 || destinoIndice >= 7) return false;

    auto& fundacaoOrigem = fundacoes[fundacaoIndice];
    auto& colDestino = colunas[destinoIndice];

    if (fundacaoOrigem.empty()) return false;

    // A carta a ser movida é a do topo da fundação
    const Carta& cartaParaMover = fundacaoOrigem.back();

    // 2. Validar o movimento para o destino escolhido
    bool movimentoValido = false;
    if (colDestino.empty()) {
        if (Regras::podeMoverParaColunaVazia(cartaParaMover)) {
            movimentoValido = true;
        }
    } else {
        if (Regras::podeMoverParaColuna(cartaParaMover, colDestino.back())) {
            movimentoValido = true;
        }
    }

    if (!movimentoValido) return false;

    // 3. Salvar estado do histórico
    salvarJogada();

    // 4. Executar a transferência da carta
    colDestino.push_back(cartaParaMover);
    fundacaoOrigem.pop_back();

    // 5. Atualizar pontuação (geralmente perde-se pontos ao retirar da fundação no Paciência clássico)
    // Se o seu sistema de pontuação tiver um evento específico para isso, altere aqui.
    // Caso contrário, ele segue o fluxo normal.
    pontuacao.aplicar(EventoPontuacao::CavaParaColuna); 

    return true;
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


std::string Paciencia::converterParaString() {
    std::string s = "";

    // 1. Estado da Cava e Descarte
    s += "C:" + std::to_string(getCavaTamanho()) + "|D:";
    if (!descarte.empty()) {
        s += std::to_string((int)descarte.back().mostraValor()) + "_" + std::to_string((int)descarte.back().mostraNaipe());
    }

    // 2. Estado das Fundações (apenas a carta do topo importa)
    s += "|F:";
    for (int i = 0; i < 4; i++) {
        if (!fundacoes[i].empty()) {
            s += std::to_string((int)fundacoes[i].back().mostraValor()) + "_" + std::to_string((int)fundacoes[i].back().mostraNaipe()) + ",";
        } else {
            s += "X,";
        }
    }

    // 3. Estado das Colunas (cartas escondidas + todas as visíveis)
    s += "|Col:";
    for (int i = 0; i < 7; i++) {
        s += std::to_string(getCartasEscondidas(i)) + "[";
        for (size_t j = getCartasEscondidas(i); j < colunas[i].size(); j++) {
            s += std::to_string((int)colunas[i][j].mostraValor()) + "_" + std::to_string((int)colunas[i][j].mostraNaipe()) + ",";
        }
        s += "];";
    }

    return s;
}

//SOLVER QUE LE TODAS AS JOGADOS POSSIVEIS 
#include <set>

// 1. MAPEIA TODAS AS JOGADAS VÁLIDAS NO TABLEIRO ATUAL
std::vector<JogadaSimulada> Paciencia::listarJogadasPossiveis() {
    std::vector<JogadaSimulada> jogadas;

    // A. Analisar cartas do Descarte
    if (!descarte.empty()) {
        const Carta& topoDescarte = descarte.back();
        
        // Descarte -> Fundação
        for (int i = 0; i < 4; i++) {
            if (Regras::podeMoverParaFundacao(topoDescarte, fundacoes[i])) {
                jogadas.push_back({"MOVER", TipoPilha::Descarte, 0, TipoPilha::Fundacao, i, 0});
            }
        }
        // Descarte -> Colunas
        for (int i = 0; i < 7; i++) {
            if (colunas[i].empty()) {
                if (Regras::podeMoverParaColunaVazia(topoDescarte))
                    jogadas.push_back({"MOVER", TipoPilha::Descarte, 0, TipoPilha::Coluna, i, 0});
            } else {
                if (Regras::podeMoverParaColuna(topoDescarte, colunas[i].back()))
                    jogadas.push_back({"MOVER", TipoPilha::Descarte, 0, TipoPilha::Coluna, i, 0});
            }
        }
    }

    // B. Analisar cartas e blocos das Colunas
    for (int i = 0; i < 7; i++) {
        if (colunas[i].empty()) continue;

        // Varre a coluna de trás para frente procurando cartas abertas
        for (int j = (int)colunas[i].size() - 1; j >= 0; j--) {
            if (!cartaVisivel(i, j)) break; // Chegou nas ocultas, para a busca nesta coluna

            const Carta& cartaAtual = colunas[i][j];
            bool ehUltimaCarta = (j == (int)colunas[i].size() - 1);

            // Mover carta/bloco para outra coluna
            for (int k = 0; k < 7; k++) {
                if (i == k) continue;

                if (colunas[k].empty()) {
                    if (Regras::podeMoverParaColunaVazia(cartaAtual)) {
                        jogadas.push_back({"MOVER_BLOCO", TipoPilha::Coluna, i, TipoPilha::Coluna, k, j});
                    }
                } else {
                    if (Regras::podeMoverParaColuna(cartaAtual, colunas[k].back())) {
                        jogadas.push_back({"MOVER_BLOCO", TipoPilha::Coluna, i, TipoPilha::Coluna, k, j});
                    }
                }
            }

            // Apenas a última carta da coluna pode ir para a Fundação
            if (ehUltimaCarta) {
                for (int k = 0; k < 4; k++) {
                    if (Regras::podeMoverParaFundacao(cartaAtual, fundacoes[k])) {
                        jogadas.push_back({"MOVER", TipoPilha::Coluna, i, TipoPilha::Fundacao, k, 0});
                    }
                }
            }
        }
    }

    // C. Analisar cartas voltando da Fundação para as Colunas
    for (int i = 0; i < 4; i++) {
        if (fundacoes[i].empty()) continue;
        const Carta& topoFundacao = fundacoes[i].back();

        for (int j = 0; j < 7; j++) {
            if (colunas[j].empty()) {
                if (Regras::podeMoverParaColunaVazia(topoFundacao))
                    jogadas.push_back({"MOVER_DA_FUNDACAO", TipoPilha::Fundacao, i, TipoPilha::Coluna, j, 0});
            } else {
                if (Regras::podeMoverParaColuna(topoFundacao, colunas[j].back()))
                    jogadas.push_back({"MOVER_DA_FUNDACAO", TipoPilha::Fundacao, i, TipoPilha::Coluna, j, 0});
            }
        }
    }

    // D. Comprar Carta da Cava (Se houver cartas na cava ou descarte para reciclar)
    if (cava.tamanho() > 0 || !descarte.empty()) {
        jogadas.push_back({"COMPRAR", TipoPilha::Descarte, 0, TipoPilha::Descarte, 0, 0});
    }

    return jogadas;
}

// 2. RECURSÃO COM BACKTRACKING: EXPLORA A ÁRVORE DE JOGADAS
bool Paciencia::simularSolucao(std::set<std::string>& estadosVisitados) {
    if (verificarVitoria()) return true;

    // Evita loops infinitos salvando e checando a "impressão digital" do jogo
    std::string estadoAtualStr = converterParaString();
    if (estadosVisitados.count(estadoAtualStr)) return false; 
    estadosVisitados.insert(estadoAtualStr);

    // Se o robô ultrapassar um limite muito alto de jogadas, assumimos como travado
    if (estadosVisitados.size() > 5000) return false;

    std::vector<JogadaSimulada> jogadas = listarJogadasPossiveis();

    for (const auto& jogada : jogadas) {
        salvarJogada(); // Salva estado atual no seu histórico para poder dar 'desfazer'

        // Executa a jogada simulada usando camelCase (batendo com seu .h)
        if (jogada.tipoAcao == "COMPRAR") {
            comprarCarta();
        } else if (jogada.tipoAcao == "MOVER") {
            mover(jogada.origemTipo, jogada.origemIdx, jogada.destinoTipo, jogada.destinoIdx);
        } else if (jogada.tipoAcao == "MOVER_BLOCO") {
            moverBloco(jogada.origemIdx, jogada.cartaIdx, jogada.destinoIdx);
        } else if (jogada.tipoAcao == "MOVER_DA_FUNDACAO") {
            moverDaFundacao(jogada.origemIdx, jogada.destinoTipo, jogada.destinoIdx);
        }

        // Continua avançando a partir desta jogada aplicada
        if (simularSolucao(estadosVisitados)) {
            return true; 
        }

        // Se o caminho deu errado ou travou lá na frente, desfaz o movimento atual
        desfazer();
    }

    return false; // Nenhuma jogada desse ponto levou à vitória
}

// 3. O FILTRO DO NOVO JOGO: GERA BARALHOS ATÉ ACHAR UM VENCÍVEL
bool Paciencia::garantirJogoVencivel() {
    int tentativas = 0;
    const int MAX_TENTATIVAS = 100;

    while (tentativas < MAX_TENTATIVAS) {
        iniciarJogo();

        // Heurística simples: conta quantas cartas estão acessíveis
        // Um jogo com pelo menos 4 jogadas imediatas possíveis é aceitável
        std::vector<JogadaSimulada> jogadas = listarJogadasPossiveis();
        
        // Filtra jogadas que não sejam apenas comprar carta
        int jogadasReais = 0;
        for (const auto& j : jogadas) {
            if (j.tipoAcao != "COMPRAR") jogadasReais++;
        }

        if (jogadasReais >= 3) {
            std::cout << "[SOLVER] Jogo com " << jogadasReais << " jogadas iniciais gerado após " << tentativas + 1 << " tentativa(s)!" << std::endl;
            return true;
        }
        tentativas++;
    }

    iniciarJogo();
    std::cout << "[SOLVER] Entregando jogo padrão." << std::endl;
    return false;
}
void Paciencia::gerarJogoReversivel() {
    std::mt19937 rng(std::random_device{}());
    
    const int MAX_TENTATIVAS = 1000;
    
    for (int tentativa = 0; tentativa < MAX_TENTATIVAS; tentativa++) {

        // 1. Cria e embaralha todas as cartas
        std::vector<Carta> todasCartas;
        std::vector<Naipe> naipes = { Naipe::Paus, Naipe::Copa, Naipe::Espada, Naipe::Ouro };
        for (auto& n : naipes)
            for (int v = 1; v <= 13; v++)
                todasCartas.push_back(Carta(static_cast<Valor>(v), n));
        std::shuffle(todasCartas.begin(), todasCartas.end(), rng);

        // 2. Limpa estado
        colunas.assign(7, vector<Carta>());
        fundacoes.assign(4, vector<Carta>());
        descarte.clear();
        while (!historico.empty()) historico.pop();
        cava = Baralho(0);
        for (int i = 0; i < 7; i++) cartasEscondidas[i] = 0;

        // 3. Distribui 28 cartas nas colunas
        int idx = 0;
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j <= i; j++) {
                colunas[i].push_back(todasCartas[idx++]);
            }
            cartasEscondidas[i] = i;
        }

        // 4. Restante vai para a cava
        while (idx < (int)todasCartas.size()) {
            cava.inserirCarta(todasCartas[idx++]);
        }

        pontuacao.resetar();
        vitoria = false;

        // ==========================================
        // HEURÍSTICAS DE QUALIDADE
        // ==========================================

        // H1: Pelo menos 1 Ás visível nas colunas
        bool temAsVisivel = false;
        for (int i = 0; i < 7; i++) {
            if (!colunas[i].empty() && colunas[i].back().mostraValor() == Valor::As) {
                temAsVisivel = true;
                break;
            }
        }
        if (!temAsVisivel) {
            if (tentativa < 3) std::cout << "[DEBUG] Falhou H1" << std::endl;
            continue;
        }

        // H2: Pelo menos 2 jogadas imediatas possíveis (excluindo comprar)
        std::vector<JogadaSimulada> jogadas = listarJogadasPossiveis();
        int jogadasReais = 0;
        for (const auto& j : jogadas)
            if (j.tipoAcao != "COMPRAR") jogadasReais++;
        if (jogadasReais < 2) {
            if (tentativa < 3) std::cout << "[DEBUG] Falhou H2: " << jogadasReais << " jogadas" << std::endl;
            continue;
        }

        // H4: Pelo menos 1 sequência válida já formada
        int sequenciasValidas = 0;
        for (int i = 0; i < 7; i++) {
            int inicio = cartasEscondidas[i];
            if ((int)colunas[i].size() - inicio < 2) continue;
            bool sequenciaOk = true;
            for (int j = (int)colunas[i].size() - 1; j > inicio; j--) {
                if (!Regras::podeMoverParaColuna(colunas[i][j], colunas[i][j-1])) {
                    sequenciaOk = false;
                    break;
                }
            }
            if (sequenciaOk) sequenciasValidas++;
        }
        if (sequenciasValidas < 1) {
            if (tentativa < 3) std::cout << "[DEBUG] Falhou H4: " << sequenciasValidas << " sequencias" << std::endl;
            continue;
        }

        // Passou em todas as heurísticas!
        std::cout << "[GERADOR] Jogo gerado após " << tentativa + 1 << " tentativa(s)! "
                  << "Jogadas: " << jogadasReais 
                  << " | Sequências: " << sequenciasValidas << std::endl;
        return;
    }

    // Fallback
    iniciarJogo();
    std::cout << "[GERADOR] Usando jogo padrão após " << MAX_TENTATIVAS << " tentativas." << std::endl;
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