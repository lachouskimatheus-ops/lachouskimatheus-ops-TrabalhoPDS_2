#include "Truco.hpp"
#include "JuizMineiroTruco.hpp"
#include <stdexcept>

Truco::Truco(JuizTruco* juiz, Baralho* baralho)
    : vira_(nullptr), baralho_(baralho), juiz_(juiz), fase_(FaseTruco::AguardandoInicio),
      pontosEquipe1_(0), pontosEquipe2_(0), valorMao_(1), jogadorAtual_(0),
      jogadorInicialMao_(0), jogadorInicialQueda_(0), numeroQueda_(1),
      vitoriasEquipe1_(0), vitoriasEquipe2_(0), vencedorPrimeiraQueda_(0),
      vencedorUltimaQueda_(0), vencedorMao_(0), vencedorPartida_(0),
      equipeQuePediu_(0), valorPedido_(0), maoTravada_(false) {
    if (juiz_ == nullptr || baralho_ == nullptr) {
        throw std::invalid_argument("Truco precisa de juiz e baralho validos");
    }
}

Truco::~Truco() {
    limparCartasDaMao();
}

bool Truco::jogadorValido(int idJogador) const {
    return idJogador >= 0 && idJogador < static_cast<int>(jogadores_.size());
}

bool Truco::equipeValida(int equipe) const {
    return equipe == 1 || equipe == 2;
}

bool Truco::equipesCompletas() const {
    int equipe1 = 0;
    int equipe2 = 0;

    for (int equipe : equipes_) {
        if (equipe == 1) ++equipe1;
        else if (equipe == 2) ++equipe2;
    }

    if (jogadores_.size() == 2) return equipe1 == 1 && equipe2 == 1;
    if (jogadores_.size() == 4) return equipe1 == 2 && equipe2 == 2;
    return false;
}

int Truco::proximoValorMao(int valorAtual) const {
    if (valorAtual == 1) return 3;
    if (valorAtual == 3) return 6;
    if (valorAtual == 6) return 9;
    if (valorAtual == 9) return 12;
    return 0;
}

int Truco::resolverVencedorDaMao(int vencedorQueda) const {
    if (vitoriasEquipe1_ == 2) return 1;
    if (vitoriasEquipe2_ == 2) return 2;

    if (numeroQueda_ == 2) {
        if (vencedorPrimeiraQueda_ == 0 && vencedorQueda != 0) return vencedorQueda;
        if (vencedorPrimeiraQueda_ != 0 && vencedorQueda == 0) return vencedorPrimeiraQueda_;
    }

    if (numeroQueda_ == 3) {
        return vencedorQueda != 0 ? vencedorQueda : vencedorPrimeiraQueda_;
    }

    return 0;
}

bool Truco::adicionarJogador(Jogador_Truco* jogador, int equipe) {
    if (jogador == nullptr || !equipeValida(equipe) ||
        fase_ != FaseTruco::AguardandoInicio || jogadores_.size() >= 4) {
        return false;
    }

    if (getQuantidadeNaEquipe(equipe) >= 2) return false;

    jogadores_.push_back(jogador);
    equipes_.push_back(equipe);

    ultimoEvento_ = "JOGADOR_ADICIONADO";
    ultimaMensagem_ = jogador->getNome() + " entrou na equipe " + std::to_string(equipe);

    return true;
}

void Truco::limparCartasDaMao() {
    for (Jogador_Truco* jogador : jogadores_) {
        if (jogador != nullptr) jogador->limparMao();
    }

    for (Carta* carta : cartasDaMao_) delete carta;

    cartasDaMao_.clear();
    jogadasDaQueda_.clear();
    vira_ = nullptr;
}

void Truco::distribuirCartas() {
    baralho_->inicializar();
    baralho_->embaralhar();

    // Verifica se o juiz instanciado é o do Truco Mineiro
    bool isMineiro = dynamic_cast<JuizMineiroTruco*>(juiz_) != nullptr;

    // Se NÃO for mineiro (ou seja, Paulista), puxa o vira normalmente
    if (!isMineiro) {
        vira_ = baralho_->puxarCarta();
        if (vira_ != nullptr) cartasDaMao_.push_back(vira_);
    } else {
        vira_ = nullptr; // Mineiro não tem vira
    }

    // Distribui as 3 cartas para os jogadores
    for (int rodada = 0; rodada < 3; ++rodada) {
        for (Jogador_Truco* jogador : jogadores_) {
            Carta* carta = baralho_->puxarCarta();

            if (carta != nullptr) {
                cartasDaMao_.push_back(carta);
                jogador->receberCarta(carta);
            }
        }
    }
}

bool Truco::iniciarPartida() {
    if ((jogadores_.size() != 2 && jogadores_.size() != 4) || !equipesCompletas()) {
        ultimaMensagem_ = "A partida precisa de equipes completas com 2 ou 4 jogadores";
        return false;
    }

    pontosEquipe1_ = 0;
    pontosEquipe2_ = 0;
    vencedorPartida_ = 0;
    jogadorInicialMao_ = 0;
    fase_ = FaseTruco::AguardandoInicio;

    return iniciarNovaMao();
}

bool Truco::iniciarNovaMao() {
    if (vencedorPartida_ != 0) {
        ultimaMensagem_ = "A partida ja foi finalizada";
        return false;
    }

    if ((jogadores_.size() != 2 && jogadores_.size() != 4) || !equipesCompletas()) {
        ultimaMensagem_ = "A partida precisa de equipes completas";
        return false;
    }

    // TRAVA DE CONCORRÊNCIA: Se os 4 clicarem juntos, só o 1º passa
    if (fase_ != FaseTruco::AguardandoInicio && fase_ != FaseTruco::MaoFinalizada) {
        ultimaMensagem_ = "Estado invalido para iniciar nova mao";
        return false;
    }

    limparCartasDaMao();
    distribuirCartas();

    maoTravada_ = pontosEquipe1_ == 11 || pontosEquipe2_ == 11;
    valorMao_ = maoTravada_ ? 3 : 1;
    jogadorInicialQueda_ = jogadorInicialMao_;
    jogadorAtual_ = jogadorInicialQueda_;
    numeroQueda_ = 1;
    vitoriasEquipe1_ = 0;
    vitoriasEquipe2_ = 0;
    vencedorPrimeiraQueda_ = 0;
    vencedorUltimaQueda_ = 0;
    vencedorMao_ = 0;
    equipeQuePediu_ = 0; // Zera para qualquer um poder pedir
    valorPedido_ = 0;
    nomePedidor_.clear();
    jogadasDaQueda_.clear();

    ultimoEvento_ = "NOVA_MAO";
    ultimaMensagem_ = maoTravada_ ? "Mao de onze iniciada valendo tres pontos" : "Nova mao iniciada";
    fase_ = FaseTruco::AguardandoJogada;
    
    return true;
}

bool Truco::podeJogar(int idJogador) const {
    return fase_ == FaseTruco::AguardandoJogada &&
           jogadorValido(idJogador) &&
           idJogador == jogadorAtual_;
}

bool Truco::podePedirTruco(int idJogador) const {
    return podeJogar(idJogador) &&
           !maoTravada_ &&
           proximoValorMao(valorMao_) != 0 &&
           getEquipeDoJogador(idJogador) != equipeQuePediu_; // A mesma equipe não pode pedir em cima do próprio pedido
}

bool Truco::podeResponderTruco(int idJogador) const {
    return fase_ == FaseTruco::AguardandoRespostaTruco &&
           jogadorValido(idJogador) &&
           getEquipeDoJogador(idJogador) != equipeQuePediu_;
}

bool Truco::jogarCarta(int idJogador, int indiceCarta) {
    if (!podeJogar(idJogador)) {
        ultimaMensagem_ = "Jogada fora de turno ou fase invalida";
        return false;
    }

    // --- ATRASO DE LIMPEZA DA MESA ---
    // Se a mesa estiver cheia (da queda anterior), limpamos apenas agora
    // que o primeiro jogador da nova queda confirmou sua jogada.
    if (jogadasDaQueda_.size() == jogadores_.size()) {
        jogadasDaQueda_.clear();
    }

    Carta* carta = jogadores_[idJogador]->jogarCarta(indiceCarta);

    if (carta == nullptr) {
        ultimaMensagem_ = "Indice de carta invalido";
        return false;
    }

    jogadasDaQueda_.push_back({idJogador, carta});
    ultimoEvento_ = "CARTA_JOGADA";
    ultimaMensagem_ = jogadores_[idJogador]->getNome() + " jogou uma carta";

    if (jogadasDaQueda_.size() == jogadores_.size()) concluirQueda();
    else avancarJogador();

    return true;
}

void Truco::avancarJogador() {
    jogadorAtual_ = (jogadorAtual_ + 1) % static_cast<int>(jogadores_.size());
}

void Truco::concluirQueda() {
    std::vector<Carta*> cartas;
    cartas.reserve(jogadasDaQueda_.size());

    for (const JogadaTruco& jogada : jogadasDaQueda_) {
        cartas.push_back(jogada.carta);
    }

    bool forcarVencedor = false;
    
    // --- PROTEÇÃO CONTRA REFERÊNCIA NULA E VAZAMENTO DE MEMÓRIA ---
    // Cria uma carta lixo na memória local (stack) apenas para enganar
    // a assinatura da função, garantindo que não deixamos lixo no ponteiro.
    Carta cartaCoringa(1, Naipe::ouros); 
    Carta* viraTemporario = vira_ != nullptr ? vira_ : &cartaCoringa;

    int indiceVencedor = juiz_->decidirVencedor(cartas, *viraTemporario, forcarVencedor);

    int jogadorVencedor = -1;
    int vencedorQueda = 0;

    if (indiceVencedor >= 0 && indiceVencedor < static_cast<int>(jogadasDaQueda_.size())) {
        jogadorVencedor = jogadasDaQueda_[indiceVencedor].idJogador;
        vencedorQueda = getEquipeDoJogador(jogadorVencedor);
    }

    vencedorUltimaQueda_ = vencedorQueda;
    ultimoEvento_ = "FIM_QUEDA";

    if (numeroQueda_ == 1) vencedorPrimeiraQueda_ = vencedorQueda;

    if (vencedorQueda == 1) ++vitoriasEquipe1_;
    else if (vencedorQueda == 2) ++vitoriasEquipe2_;

    int vencedorMao = resolverVencedorDaMao(vencedorQueda);

    if (vencedorMao != 0 || numeroQueda_ == 3) {
        finalizarMao(vencedorMao, vencedorMao == 0 ? 0 : valorMao_);
        return;
    }

    int proximoInicial = vencedorQueda == 0 ? jogadorInicialQueda_ : jogadorVencedor;
    iniciarProximaQueda(proximoInicial);
}

void Truco::iniciarProximaQueda(int jogadorInicial) {
    ++numeroQueda_;
    
    // A limpeza de jogadasDaQueda_.clear() foi removida daqui!
    // Ela agora será feita no início da próxima jogada para garantir
    // que a interface receba o estado do jogo com as 4 cartas na mesa.

    jogadorInicialQueda_ = jogadorInicial;
    jogadorAtual_ = jogadorInicial;
    fase_ = FaseTruco::AguardandoJogada;
    
    // Dispara a flag que o Front-end vai interceptar
    ultimoEvento_ = "FIM_QUEDA";
    ultimaMensagem_ = vencedorUltimaQueda_ == 0 
        ? "A queda empatou!" 
        : "Equipe " + std::to_string(vencedorUltimaQueda_) + " levou a queda!";
}

bool Truco::pedirTruco(int idJogador) {
    if (!podePedirTruco(idJogador)) {
        ultimaMensagem_ = maoTravada_
            ? "Nao e permitido pedir truco na mao de onze"
            : "Nao e possivel pedir aumento agora";

        return false;
    }

    equipeQuePediu_ = getEquipeDoJogador(idJogador);
    valorPedido_ = proximoValorMao(valorMao_);
    nomePedidor_ = jogadores_[idJogador]->getNome();
    fase_ = FaseTruco::AguardandoRespostaTruco;
    ultimoEvento_ = "TRUCO_PEDIDO";
    ultimaMensagem_ = nomePedidor_ + " pediu aumento";

    return true;
}

bool Truco::aceitarTruco(int idJogador) {
    if (!podeResponderTruco(idJogador)) {
        ultimaMensagem_ = "Este jogador nao pode responder ao pedido";
        return false;
    }

    valorMao_ = valorPedido_;
    // A equipeQuePediu_ NÃO é zerada, pois ela retém o poder de quem aceitou!
    valorPedido_ = 0;
    nomePedidor_.clear();
    fase_ = FaseTruco::AguardandoJogada;
    ultimoEvento_ = "TRUCO_ACEITO";
    ultimaMensagem_ = "Pedido aceito";

    return true;
}

bool Truco::recusarTruco(int idJogador) {
    if (!podeResponderTruco(idJogador)) {
        ultimaMensagem_ = "Este jogador nao pode recusar o pedido";
        return false;
    }

    int equipeVencedora = equipeQuePediu_;
    finalizarMao(equipeVencedora, valorMao_);

    if (vencedorPartida_ == 0) ultimoEvento_ = "TRUCO_RECUSADO";

    ultimaMensagem_ = "Pedido recusado";
    return true;
}

bool Truco::aumentarTruco(int idJogador) {
    if (!podeResponderTruco(idJogador)) {
        ultimaMensagem_ = "Este jogador nao pode aumentar o pedido";
        return false;
    }

    int novoPedido = proximoValorMao(valorPedido_);

    if (novoPedido == 0) {
        ultimaMensagem_ = "Nao e possivel aumentar alem de doze";
        return false;
    }

    valorMao_ = valorPedido_;
    equipeQuePediu_ = getEquipeDoJogador(idJogador);
    valorPedido_ = novoPedido;
    nomePedidor_ = jogadores_[idJogador]->getNome();
    fase_ = FaseTruco::AguardandoRespostaTruco;
    ultimoEvento_ = "TRUCO_PEDIDO";
    ultimaMensagem_ = nomePedidor_ + " aumentou o pedido";

    return true;
}

void Truco::finalizarMao(int equipeVencedora, int pontosGanhos) {
    vencedorMao_ = equipeVencedora;

    if (equipeVencedora == 1) pontosEquipe1_ += pontosGanhos;
    else if (equipeVencedora == 2) pontosEquipe2_ += pontosGanhos;

    equipeQuePediu_ = 0;
    valorPedido_ = 0;
    nomePedidor_.clear();

    if (pontosEquipe1_ >= 12 || pontosEquipe2_ >= 12) {
        vencedorPartida_ = pontosEquipe1_ >= 12 ? 1 : 2;
        fase_ = FaseTruco::PartidaFinalizada;
        ultimoEvento_ = "FIM_PARTIDA";
        ultimaMensagem_ = vencedorPartida_ == 1
            ? "Equipe 1 venceu a partida"
            : "Equipe 2 venceu a partida";

        return;
    }

    jogadorInicialMao_ =
        (jogadorInicialMao_ + 1) % static_cast<int>(jogadores_.size());

    fase_ = FaseTruco::MaoFinalizada;
    ultimoEvento_ = "FIM_MAO";
    ultimaMensagem_ = equipeVencedora == 0
        ? "A mao terminou empatada"
        : "Equipe " + std::to_string(equipeVencedora) + " venceu a mao";
}

bool Truco::equipeDisponivel(int equipe, int maxJogadores) const {
    if (!equipeValida(equipe)) return false;
    if (maxJogadores != 2 && maxJogadores != 4) return false;

    return getQuantidadeNaEquipe(equipe) < maxJogadores / 2;
}

int Truco::getQuantidadeNaEquipe(int equipe) const {
    int quantidade = 0;

    for (int equipeJogador : equipes_) {
        if (equipeJogador == equipe) ++quantidade;
    }

    return quantidade;
}

int Truco::getProximoValorMao() const {
    if (fase_ == FaseTruco::AguardandoRespostaTruco) return valorPedido_;
    return proximoValorMao(valorMao_);
}

int Truco::getJogadorInicialQueda() const {
    return jogadorInicialQueda_;
}

int Truco::getVencedorPrimeiraQueda() const {
    return vencedorPrimeiraQueda_;
}

int Truco::getVitoriasEquipe1() const {
    return vitoriasEquipe1_;
}

int Truco::getVitoriasEquipe2() const {
    return vitoriasEquipe2_;
}

int Truco::getQuantidadeJogadores() const {
    return static_cast<int>(jogadores_.size());
}

int Truco::getEquipeDoJogador(int idJogador) const {
    return jogadorValido(idJogador) ? equipes_[idJogador] : 0;
}

FaseTruco Truco::getFase() const {
    return fase_;
}

int Truco::getJogadorAtual() const {
    return jogadorAtual_;
}

int Truco::getNumeroQueda() const {
    return numeroQueda_;
}

int Truco::getValorMao() const {
    return valorMao_;
}

int Truco::getPontosEquipe1() const {
    return pontosEquipe1_;
}

int Truco::getPontosEquipe2() const {
    return pontosEquipe2_;
}

int Truco::getVencedorUltimaQueda() const {
    return vencedorUltimaQueda_;
}

int Truco::getVencedorMao() const {
    return vencedorMao_;
}

int Truco::getVencedorPartida() const {
    return vencedorPartida_;
}

int Truco::getEquipeQuePediu() const {
    return equipeQuePediu_;
}

int Truco::getValorPedido() const {
    return valorPedido_;
}

bool Truco::getMaoTravada() const {
    return maoTravada_;
}

const std::string& Truco::getNomePedidor() const {
    return nomePedidor_;
}

const std::string& Truco::getUltimoEvento() const {
    return ultimoEvento_;
}

const std::string& Truco::getUltimaMensagem() const {
    return ultimaMensagem_;
}

const Carta* Truco::getVira() const {
    return vira_;
}

const std::vector<JogadaTruco>& Truco::getJogadasDaQueda() const {
    return jogadasDaQueda_;
}

const std::vector<Jogador_Truco*>& Truco::getJogadores() const {
    return jogadores_;
}