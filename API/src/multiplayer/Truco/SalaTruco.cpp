#include "multiplayer/Truco/SalaTruco.hpp"
#include "JuizMineiroTruco.hpp"
#include "JuizPaulistaTruco.hpp"

SalaTruco::SalaTruco(const std::string& idSala, TipoTruco tipo, int maxJogadores)
    : SalaBase(idSala, maxJogadores), tipo_(tipo), partidaIniciada_(false) {
    if (tipo_ == TipoTruco::Mineiro) {
        juiz_ = std::make_unique<JuizMineiroTruco>();
    } else {
        juiz_ = std::make_unique<JuizPaulistaTruco>();
    }

    jogo_ = std::make_unique<Truco>(juiz_.get(), &baralho_);
}

ConexaoTruco* SalaTruco::buscarConexaoDoJogador(int idJogador) {
    for (ConexaoTruco& registro : conexoes_) {
        if (registro.idJogador == idJogador) return &registro;
    }

    return nullptr;
}

const ConexaoTruco* SalaTruco::buscarConexaoDoJogador(int idJogador) const {
    for (const ConexaoTruco& registro : conexoes_) {
        if (registro.idJogador == idJogador) return &registro;
    }

    return nullptr;
}

int SalaTruco::adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome, int equipe) {
    if (conexao == nullptr || tokenReconexao.empty() || nome.empty()) return -1;

    int idExistente = obterIdJogador(conexao);
    if (idExistente != -1) return idExistente;

    if (tokenExiste(tokenReconexao)) return reconectarJogador(conexao, tokenReconexao);
    if (!podeReceberNovoJogador()) return -1;
    if (!equipeDisponivel(equipe)) return -1;

    int idJogador = SalaBase::adicionarJogador(tokenReconexao);
    if (idJogador == -1) return -1;

    auto jogador = std::make_unique<Jogador_Truco>(nome);

    if (!jogo_->adicionarJogador(jogador.get(), equipe)) {
        SalaBase::desconectarJogador(idJogador);
        return -1;
    }

    jogadoresTruco_.push_back(std::move(jogador));
    conexoes_.push_back({idJogador, conexao});

    if (todosConectados()) iniciarPartida();

    return idJogador;
}

int SalaTruco::reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao) {
    if (conexao == nullptr || tokenReconexao.empty()) return -1;

    int idJogador = SalaBase::reconectarJogador(tokenReconexao);
    if (idJogador == -1) return -1;

    ConexaoTruco* registro = buscarConexaoDoJogador(idJogador);

    if (registro != nullptr) registro->conexao = conexao;
    else conexoes_.push_back({idJogador, conexao});

    return idJogador;
}

bool SalaTruco::removerConexao(crow::websocket::connection* conexao) {
    if (conexao == nullptr) return false;

    for (auto it = conexoes_.begin(); it != conexoes_.end(); ++it) {
        if (it->conexao != conexao) continue;

        int idJogador = it->idJogador;
        conexoes_.erase(it);

        return SalaBase::desconectarJogador(idJogador);
    }

    return false;
}

bool SalaTruco::iniciarPartida() {
    if (partidaIniciada_) return true;
    if (!todosConectados()) return false;

    partidaIniciada_ = jogo_->iniciarPartida();

    return partidaIniciada_;
}

bool SalaTruco::partidaIniciada() const {
    return partidaIniciada_;
}

bool SalaTruco::podeReceberNovoJogador() const {
    return !partidaIniciada_ && podeAdicionarNovoJogador();
}

bool SalaTruco::podeReconectar(const std::string& tokenReconexao) const {
    return tokenExiste(tokenReconexao);
}

bool SalaTruco::equipeDisponivel(int equipe) const {
    if (equipe != 1 && equipe != 2) return false;

    int limite = maxJogadores_ == 2 ? 1 : 2;

    return quantidadeNaEquipe(equipe) < limite;
}

int SalaTruco::obterIdJogador(crow::websocket::connection* conexao) const {
    if (conexao == nullptr) return -1;

    for (const ConexaoTruco& registro : conexoes_) {
        if (registro.conexao == conexao) return registro.idJogador;
    }

    return -1;
}

int SalaTruco::quantidadeNaEquipe(int equipe) const {
    if (!jogo_) return 0;

    int quantidade = 0;

    for (int i = 0; i < static_cast<int>(jogadoresTruco_.size()); ++i) {
        if (jogo_->getEquipeDoJogador(i) == equipe) ++quantidade;
    }

    return quantidade;
}

crow::websocket::connection* SalaTruco::obterConexaoJogador(int idJogador) const {
    const ConexaoTruco* registro = buscarConexaoDoJogador(idJogador);
    return registro == nullptr ? nullptr : registro->conexao;
}

TipoTruco SalaTruco::tipo() const {
    return tipo_;
}

Truco& SalaTruco::jogo() {
    return *jogo_;
}

const Truco& SalaTruco::jogo() const {
    return *jogo_;
}

Jogador_Truco* SalaTruco::jogadorTruco(int idJogador) {
    if (idJogador < 0 || idJogador >= static_cast<int>(jogadoresTruco_.size())) return nullptr;
    return jogadoresTruco_[idJogador].get();
}

const Jogador_Truco* SalaTruco::jogadorTruco(int idJogador) const {
    if (idJogador < 0 || idJogador >= static_cast<int>(jogadoresTruco_.size())) return nullptr;
    return jogadoresTruco_[idJogador].get();
}

std::vector<ConexaoTruco>& SalaTruco::conexoes() {
    return conexoes_;
}

const std::vector<ConexaoTruco>& SalaTruco::conexoes() const {
    return conexoes_;
}