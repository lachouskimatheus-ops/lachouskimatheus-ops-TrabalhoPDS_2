#include "multiplayer/FDP/SalaFDP.hpp"
#include "coreAPI/JsonConversor.hpp"
#include "JogadorFDP.hpp"

SalaFDP::SalaFDP(const std::string& idSala, int maxJogadores)
    : SalaBase(idSala, maxJogadores),
      baralho_(),
      placar_(),
      mesa_(&baralho_, &placar_),
      partidaIniciada_(false),
      processandoFimVaza_(false) {
}

int SalaFDP::adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao) {
    if (conexao == nullptr) return -1;

    int idExistente = buscarJogadorPorToken(tokenReconexao);
    bool reconexao = idExistente != -1;

    int idJogador = SalaBase::adicionarJogador(tokenReconexao);
    if (idJogador == -1) return -1;

    if (!reconexao) {
        std::string nome = "Jogador " + std::to_string(idJogador + 1);
        mesa_.adicionarJogador(new JogadorFDP(idJogador, nome));
    }

    for (ConexaoFDP& registro : conexoes_) {
        if (registro.idJogador == idJogador) {
            registro.conexao = conexao;

            if (todosConectados() && !partidaIniciada_) {
                mesa_.iniciarPartida();
                partidaIniciada_ = true;
            }

            return idJogador;
        }
    }

    conexoes_.push_back({idJogador, conexao});

    if (todosConectados() && !partidaIniciada_) {
        mesa_.iniciarPartida();
        partidaIniciada_ = true;
    }

    return idJogador;
}

bool SalaFDP::removerConexao(crow::websocket::connection* conexao) {
    if (conexao == nullptr) return false;

    for (ConexaoFDP& registro : conexoes_) {
        if (registro.conexao == conexao) {
            registro.conexao = nullptr;
            return desconectarJogador(registro.idJogador);
        }
    }

    return false;
}

int SalaFDP::obterIdJogador(crow::websocket::connection* conexao) const {
    if (conexao == nullptr) return -1;

    for (const ConexaoFDP& registro : conexoes_) {
        if (registro.conexao == conexao) return registro.idJogador;
    }

    return -1;
}

crow::websocket::connection* SalaFDP::obterConexaoJogador(int idJogador) const {
    for (const ConexaoFDP& registro : conexoes_) {
        if (registro.idJogador == idJogador) return registro.conexao;
    }

    return nullptr;
}

bool SalaFDP::possuiConexao(crow::websocket::connection* conexao) const {
    return obterIdJogador(conexao) != -1;
}

bool SalaFDP::podeReceberNovoJogador() const {
    return !partidaIniciada_ && podeAdicionarNovoJogador();
}

bool SalaFDP::podeReconectar(const std::string& tokenReconexao) const {
    int idJogador = buscarJogadorPorToken(tokenReconexao);
    return idJogador != -1 && !jogadorEstaConectado(idJogador);
}

bool SalaFDP::partidaIniciada() const {
    return partidaIniciada_;
}

bool SalaFDP::jogarCarta(int idJogador, int indiceCarta) {
    if (!partidaIniciada_) return false;

    JogadorFDP* jogadorDaVez = mesa_.getJogadorDaVez();

    if (jogadorDaVez == nullptr || jogadorDaVez->getId() != idJogador) {
        return false;
    }

    return mesa_.jogarCarta(indiceCarta);
}

bool SalaFDP::apostar(int idJogador, int valor) {
    if (!partidaIniciada_) return false;

    JogadorFDP* jogadorDaVez = mesa_.getJogadorDaVez();

    if (jogadorDaVez == nullptr || jogadorDaVez->getId() != idJogador) {
        return false;
    }

    if (!mesa_.registrarAposta(valor)) return false;

    if (mesa_.faseApostasFinalizada()) {
        mesa_.iniciarFaseDeCartas();
    }

    return true;
}

json SalaFDP::gerarJson(int idJogador) const {
    json estado = JsonConversor::mesaFdpParaJson(mesa_, idJogador);

    estado["tipo"] = "ESTADO_JOGO";
    estado["sala"] = idSala_;
    estado["meu_id"] = idJogador;
    estado["max_jogadores"] = maxJogadores_;
    estado["jogadores_conectados"] = jogadoresConectados_;
    estado["jogadores_registrados"] = jogadoresRegistrados();
    estado["partida_iniciada"] = partidaIniciada_;

    return estado;
}

bool SalaFDP::vazaFinalizada(){
    return mesa_.vazaFinalizada();
}

bool SalaFDP::rodadaFinalizada() const {
    return mesa_.rodadaFinalizada();
}

void SalaFDP::finalizarVaza() {
    mesa_.apurarVencedorDaVaza();
}

void SalaFDP::finalizarRodada() {
    mesa_.finalizarRodada();
}

bool SalaFDP::processandoFimVaza() const {
    return processandoFimVaza_;
}

void SalaFDP::definirProcessandoFimVaza(bool processando) {
    processandoFimVaza_ = processando;
}

const std::vector<ConexaoFDP>& SalaFDP::conexoes() const {
    return conexoes_;
}