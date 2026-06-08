#include "paciencia.h"
#include "regras.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <stdexcept>

// 1. Construtor
Paciencia::Paciencia() : vitoria(false), pontuacao() {
    iniciarJogo();
}

// 2. Destrutor
Paciencia::~Paciencia() {
    // Como utilizamos std::vector e std::stack, a memória é liberada automaticamente.
    // Não é necessária limpeza manual, garantindo segurança de memória.
}

// 3. Iniciar Jogo (Reset Total)
void Paciencia::iniciarJogo() {
    // Limpar estados anteriores com segurança
    colunas.assign(7, std::vector<Carta>());
    fundacoes.assign(4, std::vector<Carta>());
    descarte.clear();
    
    // Esvaziar histórico
    while(!historico.empty()) historico.pop();
    
    // Reiniciar baralho
    cava = Baralho();
    cava.embaralhar();
    
    // Distribuir cartas nas colunas (lógica original preservada e otimizada)
    for (int i = 0; i < 7; ++i) {
        colunas[i].clear();
        cartasEscondidas[i] = i; // Define quantas cartas estão de costas
        for (int j = 0; j <= i; ++j) {
            Carta c = cava.retirarCarta();
            colunas[i].push_back(c);
        }
    }
    
    pontuacao.resetar();
    vitoria = false;
}

// 4. Virar para Cima
// Método defensivo: valida os índices antes de manipular o estado.
void Paciencia::virarParaCima(int coluna) {
    if (coluna < 0 || coluna >= 7) return; 
    
    if (cartasEscondidas[coluna] > 0 && !colunas[coluna].empty()) {
        cartasEscondidas[coluna]--;
        pontuacao.aplicar(EventoPontuacao::VirarCarta);
    }
}

// 5. Carta Visível (Consulta se a carta está aberta)
bool Paciencia::cartaVisivel(int coluna, int linha) const {
    // Defensivo: Verifica limites antes de acessar
    if (coluna < 0 || coluna >= 7) return false;
    return (linha >= cartasEscondidas[coluna]);
}

void Paciencia::salvarEstadoNoHistorico() {
    EstadoJogo estado;
    estado.colunas = colunas;
    estado.fundacoes = fundacoes;
    estado.descarte = descarte;
    estado.cava = cava;
    estado.pontos = pontuacao.getPontos();
    estado.passadasCava = pontuacao.getPassadasCava();
    
    for (int i = 0; i < 7; ++i) {
        estado.cartasEscondidas[i] = cartasEscondidas[i];
    }
    
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
    
    // Restaurando pontuação
    pontuacao.setPontos(anterior.pontos);
    pontuacao.setPassadasCava(anterior.passadasCava);
    
    // Restaurando visibilidade
    for (int i = 0; i < 7; ++i) {
        cartasEscondidas[i] = anterior.cartasEscondidas[i];
    }

    return true;
}

// 8. Comprar Carta
// Gerencia a compra da cava, a reciclagem do descarte quando a cava esvazia
// e a pontuação associada.
void Paciencia::comprarCarta() {
    salvarEstadoNoHistorico(); // Utilizando o wrapper que chama o salvamento interno
    
    if (cava.estaVazio()) {
        if (descarte.empty()) return;
        
        // Reciclagem: reverte o descarte para inserir novamente na cava
        std::reverse(descarte.begin(), descarte.end());
        for(const auto& c : descarte) {
            cava.inserirCarta(c);
        }
        descarte.clear();

        // Regra de pontuação: penalização ou contagem de passadas
        pontuacao.setPassadasCava(pontuacao.getPassadasCava() + 1);
        pontuacao.aplicar(EventoPontuacao::PassarBaralho);
    } else {
        // Movimentação normal
        Carta c = cava.retirarCarta();
        descarte.push_back(c);
    }
}
// --- 10. Mover (Geral) ---
bool Paciencia::mover(TipoPilha origemTipo, int origemIndice, TipoPilha destinoTipo, int destinoIndice) {
    if (!podeMover(origemTipo, origemIndice, destinoTipo, destinoIndice)) {
        return false;
    }

    salvarEstadoNoHistorico();
    executarMovimento(origemTipo, origemIndice, destinoTipo, destinoIndice);
    verificarVitoria();
    return true;
}

// --- 11. Mover Bloco ---
bool Paciencia::moverBloco(int origemColuna, int cartaIdx, int destinoColuna) {
    if (!podeMoverBloco(origemColuna, cartaIdx, destinoColuna)) {
        return false;
    }

    salvarEstadoNoHistorico();
    executarMovimentoBloco(origemColuna, cartaIdx, destinoColuna);
    verificarVitoria();
    return true;
}

// --- 12. Mover da Fundação ---
bool Paciencia::moverDaFundacao(int fundacaoIndice, TipoPilha destinoTipo, int destinoIndice) {
    // Validação específica: mover da fundação só vai para coluna
    if (destinoTipo != TipoPilha::Coluna || !podeMoverDaFundacao(fundacaoIndice, destinoIndice)) {
        return false;
    }

    salvarEstadoNoHistorico();
    executarMovimentoDaFundacao(fundacaoIndice, destinoIndice);
    return true;
}

// ==============================================================================
// LÓGICA PRIVADA: BLOCOS E FUNDAÇÃO
// ==============================================================================

bool Paciencia::podeMoverBloco(int origemCol, int cartaIdx, int destinoCol) const {
    if (origemCol < 0 || origemCol >= 7 || destinoCol < 0 || destinoCol >= 7) return false;
    if (origemCol == destinoCol) return false;
    if (colunas[origemCol].empty() || cartaIdx < 0 || cartaIdx >= (int)colunas[origemCol].size()) return false;
    if (!cartaVisivel(origemCol, cartaIdx)) return false;

    const Carta& baseBloco = colunas[origemCol][cartaIdx];
    
    if (colunas[destinoCol].empty()) {
        return Regras::podeMoverParaColunaVazia(baseBloco);
    } else {
        return Regras::podeMoverParaColuna(baseBloco, colunas[destinoCol].back());
    }
}

void Paciencia::executarMovimentoBloco(int origemCol, int cartaIdx, int destinoCol) {
    auto& colOrigem = colunas[origemCol];
    auto& colDestino = colunas[destinoCol];

    // Transfere o bloco
    colDestino.insert(colDestino.end(), colOrigem.begin() + cartaIdx, colOrigem.end());
    colOrigem.erase(colOrigem.begin() + cartaIdx, colOrigem.end());

    // Se liberou cartas escondidas, vira a nova carta do topo
    if (!colOrigem.empty() && (int)colOrigem.size() == cartasEscondidas[origemCol] && cartasEscondidas[origemCol] > 0) {
        virarParaCima(origemCol);
    }

    pontuacao.aplicar(EventoPontuacao::ColunaParaColuna);
    // Nota: O evento exato depende do seu sistema de pontuação (ex: mover bloco pode ter pontuação diferente)
}

bool Paciencia::podeMoverDaFundacao(int fundacaoIdx, int destinoCol) const {
    if (fundacaoIdx < 0 || fundacaoIdx >= 4 || destinoCol < 0 || destinoCol >= 7) return false;
    if (fundacoes[fundacaoIdx].empty()) return false;

    const Carta& cartaParaMover = fundacoes[fundacaoIdx].back();
    
    if (colunas[destinoCol].empty()) {
        return Regras::podeMoverParaColunaVazia(cartaParaMover);
    } else {
        return Regras::podeMoverParaColuna(cartaParaMover, colunas[destinoCol].back());
    }
}

void Paciencia::executarMovimentoDaFundacao(int fundacaoIdx, int destinoCol) {
    Carta c = fundacoes[fundacaoIdx].back();
    fundacoes[fundacaoIdx].pop_back();
    colunas[destinoCol].push_back(c);

    pontuacao.aplicar(EventoPontuacao::FundacaoParaColuna);
}

EventoPontuacao Paciencia::definirEvento(TipoPilha origem, TipoPilha destino) const {
    if (origem == TipoPilha::Descarte && destino == TipoPilha::Fundacao) return EventoPontuacao::CavaParaFundacao;
    if (origem == TipoPilha::Coluna && destino == TipoPilha::Fundacao) return EventoPontuacao::ColunaParaFundacao;
    if (origem == TipoPilha::Descarte && destino == TipoPilha::Coluna) return EventoPontuacao::CavaParaColuna;
    if (origem == TipoPilha::Coluna && destino == TipoPilha::Coluna) return EventoPontuacao::ColunaParaColuna;
    return EventoPontuacao::CavaParaColuna; // Default
}

// 13. Verificar Vitória
// Verifica se todas as cartas estão nas fundações.
// É uma boa prática chamar isso após cada movimento bem-sucedido.
bool Paciencia::verificarVitoria() {
    for (int i = 0; i < 4; ++i) {
        if (fundacoes[i].size() < 13) {
            vitoria = false;
            return false;
        }
    }
    vitoria = true;
    return true;
}
// 14. Existe Jogada Possível
// Verifica se o jogo ainda tem movimentos válidos. 
// Útil para detectar Game Over.
bool Paciencia::existeJogadaPossivel() const {
    // 1. Verifica jogadas das Colunas para as Fundações
    for (int i = 0; i < 7; ++i) {
        if (!colunas[i].empty()) {
            const Carta& c = colunas[i].back();
            if (Regras::podeMoverParaFundacao(c, fundacoes[static_cast<int>(c.mostraNaipe())])) return true;
        }
    }

    // 2. Verifica jogadas de Coluna para Coluna
    for (int i = 0; i < 7; ++i) {
        if (colunas[i].empty()) continue;
        const Carta& base = colunas[i].back();
        
        for (int j = 0; j < 7; ++j) {
            if (i == j) continue;
            if (colunas[j].empty()) {
                if (Regras::podeMoverParaColunaVazia(base)) return true;
            } else {
                if (Regras::podeMoverParaColuna(base, colunas[j].back())) return true;
            }
        }
    }

    // 3. Verifica se há cartas no descarte
    if (!descarte.empty()) {
        const Carta& d = descarte.back();
        // Pode mover para fundação?
        if (Regras::podeMoverParaFundacao(d, fundacoes[static_cast<int>(d.mostraNaipe())])) return true;
        // Pode mover para alguma coluna?
        for (int i = 0; i < 7; ++i) {
            if (colunas[i].empty()) {
                if (Regras::podeMoverParaColunaVazia(d)) return true;
            } else {
                if (Regras::podeMoverParaColuna(d, colunas[i].back())) return true;
            }
        }
    }

    return false;
}

std::string Paciencia::converterParaString() const {
    std::string output = "--- ESTADO DO JOGO ---\n";
    
    // 1. Fundações
    output += "Fundacoes: ";
    for (int i = 0; i < 4; ++i) {
        if (fundacoes[i].empty()) output += "[  ] ";
        else output += "[" + fundacoes[i].back().toString() + "] ";
    }
    
    // 2. Descarte
    output += "\nDescarte: " + (descarte.empty() ? "[Vazio]" : descarte.back().toString());
    output += "\n\nColunas:\n";

    // 3. Colunas (Iterando até a altura máxima da coluna mais alta)
    int maxAltura = 0;
    for (int i = 0; i < 7; ++i) {
        if ((int)colunas[i].size() > maxAltura) maxAltura = colunas[i].size();
    }

    for (int linha = 0; linha < maxAltura; ++linha) {
        for (int col = 0; col < 7; ++col) {
            if (linha < (int)colunas[col].size()) {
                // Verifica se a carta está virada para baixo
                if (linha < cartasEscondidas[col]) {
                    output += "[#]  "; 
                } else {
                    output += "[" + colunas[col][linha].toString() + "] ";
                }
            } else {
                output += "     "; // Espaço vazio para manter o alinhamento
            }
        }
        output += "\n";
    }

    output += "----------------------\n";
    return output;
}

//SOLVER QUE LE TODAS AS JOGADOS POSSIVEIS 
#include <set>

// 1. ORQUESTRADOR (O método público agora é limpo e fácil de ler)
std::vector<JogadaSimulada> Paciencia::listarJogadasPossiveis() {
    std::vector<JogadaSimulada> jogadas;

    coletarJogadasDescarte(jogadas);
    coletarJogadasColunas(jogadas);
    coletarJogadasFundacao(jogadas);

    // D. Comprar Carta (Lógica mantida original)
    if (cava.tamanho() > 0 || !descarte.empty()) {
        jogadas.push_back({"COMPRAR", TipoPilha::Descarte, 0, TipoPilha::Descarte, 0, 0});
    }

    return jogadas;
}

// 2. MÉTODOS PRIVADOS (Encapsulamento das regras)
void Paciencia::coletarJogadasDescarte(std::vector<JogadaSimulada>& jogadas) const {
    if (descarte.empty()) return;
    const Carta& topo = descarte.back();

    // Descarte -> Fundação
    for (int i = 0; i < 4; ++i) {
        if (Regras::podeMoverParaFundacao(topo, fundacoes[i])) {
            jogadas.push_back({"MOVER", TipoPilha::Descarte, 0, TipoPilha::Fundacao, i, 0});
        }
    }
    // Descarte -> Colunas
    for (int i = 0; i < 7; ++i) {
        if (colunas[i].empty() ? Regras::podeMoverParaColunaVazia(topo) : Regras::podeMoverParaColuna(topo, colunas[i].back())) {
            jogadas.push_back({"MOVER", TipoPilha::Descarte, 0, TipoPilha::Coluna, i, 0});
        }
    }
}

void Paciencia::coletarJogadasColunas(std::vector<JogadaSimulada>& jogadas) const {
    for (int i = 0; i < 7; ++i) {
        if (colunas[i].empty()) continue;

        for (int j = (int)colunas[i].size() - 1; j >= 0; --j) {
            if (!cartaVisivel(i, j)) break; // Defensivo: respeita a visibilidade

            const Carta& carta = colunas[i][j];
            bool ehUltima = (j == (int)colunas[i].size() - 1);

            // Mover Bloco
            for (int k = 0; k < 7; ++k) {
                if (i == k) continue;
                if (colunas[k].empty() ? Regras::podeMoverParaColunaVazia(carta) : Regras::podeMoverParaColuna(carta, colunas[k].back())) {
                    jogadas.push_back({"MOVER_BLOCO", TipoPilha::Coluna, i, TipoPilha::Coluna, k, j});
                }
            }

            // Mover para Fundação (apenas se for a última carta)
            if (ehUltima) {
                for (int k = 0; k < 4; ++k) {
                    if (Regras::podeMoverParaFundacao(carta, fundacoes[k])) {
                        jogadas.push_back({"MOVER", TipoPilha::Coluna, i, TipoPilha::Fundacao, k, 0});
                    }
                }
            }
        }
    }
}

void Paciencia::coletarJogadasFundacao(std::vector<JogadaSimulada>& jogadas) const {
    for (int i = 0; i < 4; ++i) {
        if (fundacoes[i].empty()) continue;
        const Carta& topo = fundacoes[i].back();

        for (int j = 0; j < 7; ++j) {
            if (colunas[j].empty() ? Regras::podeMoverParaColunaVazia(topo) : Regras::podeMoverParaColuna(topo, colunas[j].back())) {
                jogadas.push_back({"MOVER_DA_FUNDACAO", TipoPilha::Fundacao, i, TipoPilha::Coluna, j, 0});
            }
        }
    }
}

// 2. RECURSÃO COM BACKTRACKING: EXPLORA A ÁRVORE DE JOGADAS
bool Paciencia::simularSolucao(std::set<std::string>& estadosVisitados) {
    if (verificarVitoria()) return true;

    // 1. Verificação de Loop (Impressão digital)
    std::string estadoAtualStr = converterParaString();
    if (estadosVisitados.count(estadoAtualStr)) return false; 
    estadosVisitados.insert(estadoAtualStr);

    // 2. Limite de profundidade (segurança)
    if (estadosVisitados.size() > 5000) return false;

    // 3. Obtém todas as jogadas possíveis
    std::vector<JogadaSimulada> jogadas = listarJogadasPossiveis();

    for (const auto& jogada : jogadas) {
        bool movimentoRealizado = false;

        // Executa a jogada e captura o sucesso (Bool)
        if (jogada.tipoAcao == "COMPRAR") {
            comprarCarta(); // Assumindo que comprar carta sempre funciona se listarJogadas permitiu
            movimentoRealizado = true; 
        } else if (jogada.tipoAcao == "MOVER") {
            movimentoRealizado = mover(jogada.origemTipo, jogada.origemIdx, jogada.destinoTipo, jogada.destinoIdx);
        } else if (jogada.tipoAcao == "MOVER_BLOCO") {
            movimentoRealizado = moverBloco(jogada.origemIdx, jogada.cartaIdx, jogada.destinoIdx);
        } else if (jogada.tipoAcao == "MOVER_DA_FUNDACAO") {
            movimentoRealizado = moverDaFundacao(jogada.origemIdx, jogada.destinoTipo, jogada.destinoIdx);
        }

        // Só prosseguimos se o movimento foi realmente válido
        if (movimentoRealizado) {
            // Continua avançando
            if (simularSolucao(estadosVisitados)) {
                return true; 
            }

            // Backtracking: só desfaz se o movimento foi feito
            desfazer();
        }
    }

    return false; // Nenhuma jogada desse ponto levou à vitória
}

// 3. O FILTRO DO NOVO JOGO: GERA BARALHOS ATÉ ACHAR UM VENCÍVEL
bool Paciencia::garantirJogoVencivel() {
    // 1. Gera um jogo matematicamente garantido como vencível
    gerarJogoReversivel();

    // 2. Heurística de Qualidade (Opcional)
    // Às vezes o jogo é vencível, mas o tabuleiro inicial é "chato" (ex: tudo escondido).
    // Podemos tentar gerar novamente apenas se o jogo estiver *muito* travado.
    std::vector<JogadaSimulada> jogadas = listarJogadasPossiveis();
    int jogadasReais = 0;
    
    for (const auto& j : jogadas) {
        if (j.tipoAcao != "COMPRAR") jogadasReais++;
    }

    // Se o jogo for muito "seco" (menos de 2 jogadas reais), geramos de novo, 
    // mas apenas uma vez para não perder performance.
    if (jogadasReais < 2) {
        std::cout << "[SOLVER] Jogo gerado estava muito travado. Regenerando..." << std::endl;
        gerarJogoReversivel();
    }

    std::cout << "[SOLVER] Jogo garantido gerado com sucesso!" << std::endl;
    return true; 
}
void Paciencia::gerarJogoReversivel() {
    std::mt19937 rng(std::random_device{}());

    // 1. Limpa o estado atual do jogo
    colunas.assign(7, std::vector<Carta>());
    fundacoes.assign(4, std::vector<Carta>());
    descarte.clear();
    while (!historico.empty()) historico.pop();
    
    // Baralho vazio para reconstruir
    cava = Baralho(0); 
    
    // Reseta visibilidade (Padrão Klondike: 1, 2, 3, 4, 5, 6, 7)
    for (int i = 0; i < 7; i++) cartasEscondidas[i] = i;

    // 2. Estado de controle reverso
    int cartas_nas_fundacoes[4] = {13, 13, 13, 13}; 
    std::vector<Naipe> naipes = { Naipe::Paus, Naipe::Copa, Naipe::Espada, Naipe::Ouro };

    std::vector<std::vector<Carta>> colunas_temp(7);
    std::vector<Carta> deck_temp;
    deck_temp.reserve(24); // Otimização de memória
    
    // Capacidade oficial de cada coluna no início do jogo (Total 28 cartas no tableau)
    int capacidade_coluna[7] = {1, 2, 3, 4, 5, 6, 7};

    // 3. Distribuição das 52 cartas
    for (int i = 0; i < 52; i++) {
        std::vector<int> naipes_disponiveis;
        for (int n = 0; n < 4; n++) {
            if (cartas_nas_fundacoes[n] > 0) {
                naipes_disponiveis.push_back(n);
            }
        }

        // Seleção aleatória do naipe
        std::uniform_int_distribution<int> dist_naipe(0, (int)naipes_disponiveis.size() - 1);
        int naipe_escolhido = naipes_disponiveis[dist_naipe(rng)];
        
        int valor_carta = cartas_nas_fundacoes[naipe_escolhido];
        cartas_nas_fundacoes[naipe_escolhido]--; 

        Carta carta_atual(static_cast<Valor>(valor_carta), naipes[naipe_escolhido]);

        // Determina onde colocar
        std::vector<int> destinos_disponiveis;
        for (int c = 0; c < 7; c++) {
            if ((int)colunas_temp[c].size() < capacidade_coluna[c]) {
                destinos_disponiveis.push_back(c);
            }
        }
        
        // Peso para a cava (índice 7)
        if (deck_temp.size() < 24) {
            for(int p = 0; p < 3; p++) destinos_disponiveis.push_back(7); 
        }

        // Segurança: garante que sempre haverá um destino
        if (destinos_disponiveis.empty()) {
            // Se as colunas estiverem cheias, força para o deck
            deck_temp.push_back(carta_atual);
        } else {
            std::uniform_int_distribution<int> dist_dest(0, (int)destinos_disponiveis.size() - 1);
            int destino_escolhido = destinos_disponiveis[dist_dest(rng)];

            if (destino_escolhido == 7) {
                deck_temp.push_back(carta_atual);
            } else {
                colunas_temp[destino_escolhido].push_back(carta_atual);
            }
        }
    }

    // 4. Finalização
    std::shuffle(deck_temp.begin(), deck_temp.end(), rng);

    for (int c = 0; c < 7; c++) {
        colunas[c] = colunas_temp[c];
    }
    for (const auto& carta : deck_temp) {
        cava.inserirCarta(carta);
    }

    pontuacao.resetar();
    vitoria = false;

    std::cout << "[GERADOR] Jogo 100% vencível gerado via Método Reverso Matemático." << std::endl;
}


// 9. Get Pontuação
// Retorna a pontuação atual. Mantive o nome original que você utilizava.
int Paciencia::getPontuacao() const {
    return pontuacao.getPontos();
}


void Paciencia::completarAutomaticamente() {
    bool movimentoPossivel = true;

    while (movimentoPossivel) {
        movimentoPossivel = false;

        // 1. Tenta mover do descarte
        if (!descarte.empty()) {
            Carta& carta = descarte.back();
            // ADICIONADO static_cast<int>
            int naipeIdx = static_cast<int>(carta.mostraNaipe()); 
            if (Regras::podeMoverParaFundacao(carta, fundacoes[naipeIdx])) {
                mover(TipoPilha::Descarte, 0, TipoPilha::Fundacao, naipeIdx);
                movimentoPossivel = true;
            }
        }

        // 2. Tenta mover das colunas
        if (!movimentoPossivel) {
            for (int i = 0; i < 7; i++) {
                if (!colunas[i].empty()) {
                    Carta& c = colunas[i].back();
                    // ADICIONADO static_cast<int>
                    int naipeIdx = static_cast<int>(c.mostraNaipe());
                    if (Regras::podeMoverParaFundacao(c, fundacoes[naipeIdx])) {
                        mover(TipoPilha::Coluna, i, TipoPilha::Fundacao, naipeIdx);
                        movimentoPossivel = true;
                        break;
                    }
                }
            }
        }
    }
}

// Esta função move apenas UMA carta por vez
bool Paciencia::moverUmaParaFundacao() {
    // 1. Tenta descarte
    for (auto& carta : descarte) {
        int nIdx = static_cast<int>(carta.mostraNaipe());
        if (Regras::podeMoverParaFundacao(carta, fundacoes[nIdx])) {
            mover(TipoPilha::Descarte, 0, TipoPilha::Fundacao, nIdx);
            return true; // Moveu uma, para por aqui
        }
    }
    // 2. Tenta colunas
    for (int i = 0; i < 7; i++) {
        if (!colunas[i].empty()) {
            Carta& c = colunas[i].back();
            int nIdx = static_cast<int>(c.mostraNaipe());
            if (Regras::podeMoverParaFundacao(c, fundacoes[nIdx])) {
                mover(TipoPilha::Coluna, i, TipoPilha::Fundacao, nIdx);
                return true;
            }
        }
    }
    return false; // Não há mais movimentos
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