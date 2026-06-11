#include "multiplayer/Poker/SalaPoker.hpp"
#include "coreAPI/JsonConversor.hpp"

SalaPoker::SalaPoker(const std::string& idSala, int maxJogadores, ModoPoker modo)
    : SalaBase(idSala, modo == ModoPoker::ContraComputador ? 1 : maxJogadores),
      jogo_(maxJogadores, modo), partidaIniciada_(false) {}

ConexaoPoker* SalaPoker::buscarConexaoDoJogador(int idJogador) {
    for (auto& registro : conexoes_) if (registro.idJogador == idJogador) return &registro;
    return nullptr;
}

const ConexaoPoker* SalaPoker::buscarConexaoDoJogador(int idJogador) const {
    for (const auto& registro : conexoes_) if (registro.idJogador == idJogador) return &registro;
    return nullptr;
}

int SalaPoker::adicionarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao, const std::string& nome) {
    if (!conexao || tokenReconexao.empty()) return -1;

    int existente = obterIdJogador(conexao);
    if (existente != -1) return existente;
    if (tokenExiste(tokenReconexao)) return reconectarJogador(conexao, tokenReconexao);
    if (!podeReceberNovoJogador()) return -1;

    int idJogador = SalaBase::adicionarJogador(tokenReconexao);
    if (idJogador == -1) return -1;

    conexoes_.push_back({idJogador, conexao});
    jogo_.definirNomeJogador(idJogador, nome);

    if (todosConectados()) iniciarPartida();
    return idJogador;
}

int SalaPoker::reconectarJogador(crow::websocket::connection* conexao, const std::string& tokenReconexao) {
    if (!conexao || tokenReconexao.empty()) return -1;

    int idJogador = SalaBase::reconectarJogador(tokenReconexao);
    if (idJogador == -1) return -1;

    ConexaoPoker* registro = buscarConexaoDoJogador(idJogador);
    if (registro) registro->conexao = conexao;
    else conexoes_.push_back({idJogador, conexao});

    return idJogador;
}

bool SalaPoker::removerConexao(crow::websocket::connection* conexao) {
    if (!conexao) return false;

    for (auto it = conexoes_.begin(); it != conexoes_.end(); ++it) {
        if (it->conexao != conexao) continue;
        int idJogador = it->idJogador;
        conexoes_.erase(it);
        return SalaBase::desconectarJogador(idJogador);
    }

    return false;
}

int SalaPoker::obterIdJogador(crow::websocket::connection* conexao) const {
    if (!conexao) return -1;
    for (const auto& registro : conexoes_) if (registro.conexao == conexao) return registro.idJogador;
    return -1;
}

crow::websocket::connection* SalaPoker::obterConexaoJogador(int idJogador) const {
    const ConexaoPoker* registro = buscarConexaoDoJogador(idJogador);
    return registro ? registro->conexao : nullptr;
}

bool SalaPoker::possuiConexao(crow::websocket::connection* conexao) const {
    return obterIdJogador(conexao) != -1;
}

bool SalaPoker::podeReceberNovoJogador() const {
    return !partidaIniciada_ && podeAdicionarNovoJogador();
}

bool SalaPoker::podeReconectar(const std::string& tokenReconexao) const {
    return tokenExiste(tokenReconexao);
}

bool SalaPoker::iniciarPartida() {
    if (partidaIniciada_ || !todosConectados()) return false;
    jogo_.iniciar();
    partidaIniciada_ = true;
    return true;
}

bool SalaPoker::partidaIniciada() const {
    return partidaIniciada_;
}

bool SalaPoker::confirmarTroca(int idJogador, const std::vector<int>& indices) {
    return partidaIniciada_ && jogadorEstaConectado(idJogador) && jogo_.confirmarTroca(idJogador, indices);
}

bool SalaPoker::iniciarNovaRodada() {
    return partidaIniciada_ && jogo_.iniciarNovaRodada();
}

std::string SalaPoker::faseParaString(FasePoker fase) {
    if (fase == FasePoker::AguardandoJogadores) return "AGUARDANDO_JOGADORES";
    if (fase == FasePoker::EscolhendoTrocas) return "ESCOLHENDO_TROCAS";
    return "RESULTADO";
}

std::string SalaPoker::modoParaString(ModoPoker modo) {
    return modo == ModoPoker::ContraComputador ? "COMPUTADOR" : "MULTIPLAYER";
}

json SalaPoker::gerarJson(int idJogadorSolicitante) const {
    json estado;
    bool idValido = jogo_.jogadorValido(idJogadorSolicitante);
    bool resultado = jogo_.fase() == FasePoker::Resultado;
    std::string fase = partidaIniciada_ ? faseParaString(jogo_.fase()) : "AGUARDANDO_JOGADORES";

    if (partidaIniciada_ && jogo_.fase() == FasePoker::EscolhendoTrocas &&
        idValido && jogo_.jogadorConfirmouTroca(idJogadorSolicitante))
        fase = "AGUARDANDO_OUTROS_JOGADORES";

    estado["tipo"] = "estado_jogo";
    estado["sala"] = idSala();
    estado["meu_id"] = idJogadorSolicitante;
    estado["modo"] = modoParaString(jogo_.modo());
    estado["fase"] = fase;
    estado["rodada"] = jogo_.rodada();
    estado["partida_iniciada"] = partidaIniciada_;
    estado["max_jogadores"] = maxJogadores();
    estado["jogadores_conectados"] = jogadoresConectados();
    estado["jogadores_registrados"] = jogadoresRegistrados();
    estado["empates"] = jogo_.empates();
    estado["vencedor"] = jogo_.vencedorRodada();
    estado["pode_confirmar_troca"] = partidaIniciada_ && idValido && jogo_.podeConfirmarTroca(idJogadorSolicitante);
    estado["pode_iniciar_nova_rodada"] = resultado && idJogadorSolicitante == 0;

    if (!partidaIniciada_) estado["mensagem"] = "Aguardando os outros jogadores entrarem.";
    else if (fase == "ESCOLHENDO_TROCAS") estado["mensagem"] = "Selecione até 3 cartas para trocar.";
    else if (fase == "AGUARDANDO_OUTROS_JOGADORES") estado["mensagem"] = "Troca confirmada. Aguardando os outros jogadores.";
    else if (jogo_.vencedorRodada() == -1) estado["mensagem"] = "A rodada terminou empatada.";
    else if (jogo_.vencedorRodada() == idJogadorSolicitante) estado["mensagem"] = "Você venceu a rodada!";
    else estado["mensagem"] = jogo_.nomeJogador(jogo_.vencedorRodada()) + " venceu a rodada.";

    estado["minha_mao"] = json::array();
    if (partidaIniciada_ && idValido)
        estado["minha_mao"] = JsonConversor::maoParaJson(jogo_.jogador(idJogadorSolicitante).verMao());

    estado["jogadores"] = json::array();

    for (int i = 0; i < jogo_.quantidadeJogadores(); i++) {
        json jogador;
        bool computador = jogo_.modo() == ModoPoker::ContraComputador && i == 1;
        bool conectado = computador || jogadorEstaConectado(i);

        jogador["id"] = i;
        jogador["nome"] = jogo_.nomeJogador(i);
        jogador["sou_eu"] = i == idJogadorSolicitante;
        jogador["computador"] = computador;
        jogador["conectado"] = conectado;
        jogador["pontos"] = jogo_.pontos()[i];
        jogador["quantidade_cartas"] = partidaIniciada_ ? jogo_.jogador(i).tamanhoMao() : 0;
        jogador["confirmou_troca"] = partidaIniciada_ && jogo_.jogadorConfirmouTroca(i);
        jogador["quantidade_ultima_troca"] = jogo_.quantidadeUltimaTroca()[i];

        if (partidaIniciada_ && (resultado || i == idJogadorSolicitante)) {
            jogador["mao"] = JsonConversor::maoParaJson(jogo_.jogador(i).verMao());
            jogador["jogada"] = jogo_.jogador(i).nomeDaJogada();
            jogador["forca"] = jogo_.jogador(i).avaliarMao();
        } else {
            jogador["mao"] = json::array();
            for (int c = 0; partidaIniciada_ && c < jogo_.jogador(i).tamanhoMao(); c++)
                jogador["mao"].push_back({{"oculta", true}});
            jogador["jogada"] = "Oculta";
            jogador["forca"] = -1;
        }

        estado["jogadores"].push_back(jogador);
    }

    return estado;
}

MesaPoker& SalaPoker::jogo() {
    return jogo_;
}

const MesaPoker& SalaPoker::jogo() const {
    return jogo_;
}

std::vector<ConexaoPoker>& SalaPoker::conexoes() {
    return conexoes_;
}

const std::vector<ConexaoPoker>& SalaPoker::conexoes() const {
    return conexoes_;
}