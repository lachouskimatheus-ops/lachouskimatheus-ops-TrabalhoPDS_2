#include "Jogo21.hpp"

#include <algorithm>
#include <stdexcept>

Jogo21::Jogo21()
    : baralho_(nullptr),
      banca_(nullptr),
      fase_(Fase21::NaoIniciado),
      rodada_(0) {
    prepararBaralho();
    banca_ = new Jogador21(0, "Banca");
}

Jogo21::~Jogo21() {
    limparPartida();

    delete baralho_;
    baralho_ = nullptr;

    delete banca_;
    banca_ = nullptr;
}

void Jogo21::prepararBaralho() {
    delete baralho_;
    baralho_ = new Baralho(1);
    baralho_->embaralhar();
}

bool Jogo21::inicializarJogo(const std::vector<std::string>& nomesLista) {
    limparPartida();

    if (nomesLista.empty()) {
        fase_ = Fase21::NaoIniciado;
        return false;
    }

    int id = 1;

    for (const std::string& nomeOriginal : nomesLista) {
        std::string nome = nomeOriginal;

        if (nome.empty()) {
            nome = "Jogador " + std::to_string(id);
        }

        jogadores_.push_back(new Jogador21(id, nome));
        id++;
    }

    resultados_.assign(jogadores_.size(), Resultado21::Indefinido);
    rodada_ = 0;

    return iniciarNovaRodada();
}

bool Jogo21::iniciarNovaRodada() {
    if (jogadores_.empty()) {
        fase_ = Fase21::NaoIniciado;
        return false;
    }

    prepararBaralho();

    for (Jogador21* jogador : jogadores_) {
        if (jogador != nullptr) {
            jogador->prepararNovaRodada();
        }
    }

    if (banca_ == nullptr) {
        banca_ = new Jogador21(0, "Banca");
    }

    banca_->prepararNovaRodada();

    resultados_.assign(jogadores_.size(), Resultado21::Indefinido);

    rodada_++;
    fase_ = Fase21::EscolhendoAcoes;

    return distribuirCartasIniciais();
}

bool Jogo21::distribuirCartasIniciais() {
    if (fase_ != Fase21::EscolhendoAcoes) {
        return false;
    }

    if (baralho_ == nullptr || banca_ == nullptr || jogadores_.empty()) {
        return false;
    }

    for (int i = 0; i < 2; i++) {
        for (Jogador21* jogador : jogadores_) {
            if (!comprarCartaPara(jogador)) {
                fase_ = Fase21::Resultado;
                determinarVencedores();
                return false;
            }
        }

        if (!comprarCartaPara(banca_)) {
            fase_ = Fase21::Resultado;
            determinarVencedores();
            return false;
        }
    }

    for (Jogador21* jogador : jogadores_) {
        if (jogador != nullptr) {
            jogador->calcularPontuacao();

            if (jogador->estourou()) {
                jogador->parar();
            }
        }
    }

    banca_->calcularPontuacao();

    return true;
}

bool Jogo21::comprarCartaPara(Jogador21* jogador) {
    if (jogador == nullptr || baralho_ == nullptr) {
        return false;
    }

    try {
        Carta* carta = baralho_->puxarCarta();

        if (carta == nullptr) {
            return false;
        }

        jogador->receberCarta(carta);
        jogador->calcularPontuacao();

        return true;
    } catch (const std::runtime_error&) {
        return false;
    }
}

int Jogo21::indiceJogadorPorId(int idJogador) const {
    for (int i = 0; i < static_cast<int>(jogadores_.size()); i++) {
        if (jogadores_[i] != nullptr && jogadores_[i]->getId() == idJogador) {
            return i;
        }
    }

    return -1;
}

bool Jogo21::podeAgir(int idJogador) const {
    int indice = indiceJogadorPorId(idJogador);

    if (indice < 0) {
        return false;
    }

    if (fase_ != Fase21::EscolhendoAcoes) {
        return false;
    }

    Jogador21* jogador = jogadores_[indice];

    if (jogador == nullptr) {
        return false;
    }

    if (jogador->parou()) {
        return false;
    }

    int pontos = const_cast<Jogador21*>(jogador)->calcularPontuacao();

    return pontos <= 21;
}

bool Jogo21::pedirCarta(int idJogador) {
    if (!podeAgir(idJogador)) {
        return false;
    }

    int indice = indiceJogadorPorId(idJogador);

    if (indice < 0) {
        return false;
    }

    Jogador21* jogador = jogadores_[indice];

    if (!comprarCartaPara(jogador)) {
        jogador->parar();
    }

    if (jogador->estourou()) {
        jogador->parar();
    }

    if (todosJogadoresFinalizaram()) {
        turnoBanca();
        determinarVencedores();
    }

    return true;
}

bool Jogo21::parar(int idJogador) {
    if (!podeAgir(idJogador)) {
        return false;
    }

    int indice = indiceJogadorPorId(idJogador);

    if (indice < 0) {
        return false;
    }

    jogadores_[indice]->parar();

    if (todosJogadoresFinalizaram()) {
        turnoBanca();
        determinarVencedores();
    }

    return true;
}

bool Jogo21::todosJogadoresFinalizaram() {
    if (jogadores_.empty()) {
        return false;
    }

    for (Jogador21* jogador : jogadores_) {
        if (jogador == nullptr) {
            continue;
        }

        if (!jogador->parou() && !jogador->estourou()) {
            return false;
        }
    }

    return true;
}

void Jogo21::turnoBanca() {
    if (banca_ == nullptr) {
        return;
    }

    fase_ = Fase21::TurnoBanca;

    while (banca_->calcularPontuacao() < 17) {
        if (!comprarCartaPara(banca_)) {
            break;
        }
    }
}

void Jogo21::determinarVencedores() {
    if (banca_ == nullptr) {
        return;
    }

    int pontosBanca = banca_->calcularPontuacao();
    bool bancaEstourou = pontosBanca > 21;

    resultados_.assign(jogadores_.size(), Resultado21::Indefinido);

    for (int i = 0; i < static_cast<int>(jogadores_.size()); i++) {
        Jogador21* jogador = jogadores_[i];

        if (jogador == nullptr) {
            resultados_[i] = Resultado21::Derrota;
            continue;
        }

        int pontosJogador = jogador->calcularPontuacao();

        if (pontosJogador > 21) {
            resultados_[i] = Resultado21::Estourou;
        } else if (bancaEstourou) {
            resultados_[i] = Resultado21::Vitoria;
        } else if (pontosJogador > pontosBanca) {
            resultados_[i] = Resultado21::Vitoria;
        } else if (pontosJogador == pontosBanca) {
            resultados_[i] = Resultado21::Empate;
        } else {
            resultados_[i] = Resultado21::Derrota;
        }
    }

    fase_ = Fase21::Resultado;
}

void Jogo21::limparPartida() {
    for (Jogador21* jogador : jogadores_) {
        if (jogador != nullptr) {
            jogador->prepararNovaRodada();
            delete jogador;
        }
    }

    jogadores_.clear();
    resultados_.clear();

    if (banca_ != nullptr) {
        banca_->prepararNovaRodada();
    }

    fase_ = Fase21::NaoIniciado;
}

const std::vector<Jogador21*>& Jogo21::getJogadores() const {
    return jogadores_;
}

Jogador21* Jogo21::getBanca() const {
    return banca_;
}

Resultado21 Jogo21::resultadoJogador(int idJogador) const {
    int indice = indiceJogadorPorId(idJogador);

    if (indice < 0 || indice >= static_cast<int>(resultados_.size())) {
        return Resultado21::Indefinido;
    }

    return resultados_[indice];
}

const std::vector<Resultado21>& Jogo21::resultados() const {
    return resultados_;
}

Fase21 Jogo21::fase() const {
    return fase_;
}

int Jogo21::rodada() const {
    return rodada_;
}

int Jogo21::quantidadeJogadores() const {
    return static_cast<int>(jogadores_.size());
}

std::string Jogo21::nomeFase() const {
    switch (fase_) {
        case Fase21::NaoIniciado:
            return "Nao iniciado";

        case Fase21::EscolhendoAcoes:
            return "Jogadores escolhendo acoes";

        case Fase21::TurnoBanca:
            return "Turno da banca";

        case Fase21::Resultado:
            return "Resultado";

        default:
            return "Fase desconhecida";
    }
}

std::string Jogo21::nomeResultado(Resultado21 resultado) {
    switch (resultado) {
        case Resultado21::Vitoria:
            return "Vitoria";

        case Resultado21::Derrota:
            return "Derrota";

        case Resultado21::Empate:
            return "Empate";

        case Resultado21::Estourou:
            return "Estourou";

        case Resultado21::Indefinido:
        default:
            return "Indefinido";
    }
}
