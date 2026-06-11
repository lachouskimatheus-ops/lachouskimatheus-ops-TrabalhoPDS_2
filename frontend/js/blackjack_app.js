let socket = null;
let estadoAtual = null;
let tokenReconexao = null;
let salaAtual = null;
let meuId = -1;
let tentativaReconexao = null;

const elementos = {};

window.addEventListener("DOMContentLoaded", inicializar);

function inicializar() {
    mapearElementos();

    salaAtual = obterParametroUrl("sala");

    if (!salaAtual) {
        mostrarMensagemEntrada("Código da sala não informado.", true);
        return;
    }

    salaAtual = salaAtual.toUpperCase();

    elementos.salaEntrada.textContent = salaAtual;
    elementos.salaJogo.textContent = salaAtual;

    tokenReconexao = obterOuCriarToken(salaAtual);

    elementos.btnConectar.addEventListener("click", conectar);
    elementos.nomeJogador.addEventListener("keydown", (evento) => {
        if (evento.key === "Enter") {
            conectar();
        }
    });

    elementos.btnPedir.addEventListener("click", pedirCarta);
    elementos.btnParar.addEventListener("click", parar);
    elementos.btnNovaRodada.addEventListener("click", novaRodada);

    atualizarBotoes(false);
}

function mapearElementos() {
    elementos.telaEntrada = document.getElementById("tela-entrada");
    elementos.telaJogo = document.getElementById("tela-jogo");

    elementos.salaEntrada = document.getElementById("sala-entrada");
    elementos.salaJogo = document.getElementById("sala-jogo");

    elementos.nomeJogador = document.getElementById("nome-jogador");
    elementos.btnConectar = document.getElementById("btn-conectar");
    elementos.mensagemEntrada = document.getElementById("mensagem-entrada");

    elementos.pontoConexao = document.getElementById("ponto-conexao");
    elementos.textoConexao = document.getElementById("texto-conexao");

    elementos.descricaoFase = document.getElementById("descricao-fase");
    elementos.rodada = document.getElementById("rodada");
    elementos.jogadoresConectados = document.getElementById("jogadores-conectados");
    elementos.faseJogo = document.getElementById("fase-jogo");

    elementos.pontosBanca = document.getElementById("pontos-banca");
    elementos.maoBanca = document.getElementById("mao-banca");

    elementos.mensagemPrincipal = document.getElementById("mensagem-principal");
    elementos.mensagemSecundaria = document.getElementById("mensagem-secundaria");

    elementos.listaJogadores = document.getElementById("lista-jogadores");

    elementos.nomeLocal = document.getElementById("nome-local");
    elementos.resultadoLocal = document.getElementById("resultado-local");
    elementos.pontosLocal = document.getElementById("pontos-local");
    elementos.maoLocal = document.getElementById("mao-local");
    elementos.statusAcao = document.getElementById("status-acao");

    elementos.btnPedir = document.getElementById("btn-pedir");
    elementos.btnParar = document.getElementById("btn-parar");
    elementos.btnNovaRodada = document.getElementById("btn-nova-rodada");
}

function conectar() {
    const nome = elementos.nomeJogador.value.trim() || "Jogador";

    elementos.btnConectar.disabled = true;
    mostrarMensagemEntrada("Conectando...");

    const protocolo = window.location.protocol === "https:" ? "wss" : "ws";
    const endereco = `${protocolo}://${window.location.host}/ws/blackjack`;

    socket = new WebSocket(endereco);

    socket.addEventListener("open", () => {
        atualizarConexao(true);

        enviar({
            tipo: "entrar_sala",
            sala: salaAtual,
            token: tokenReconexao,
            nome
        });
    });

    socket.addEventListener("message", (evento) => {
        processarMensagem(evento.data);
    });

    socket.addEventListener("close", () => {
        atualizarConexao(false);
        atualizarBotoes(false);

        if (estadoAtual !== null) {
            tentarReconectar();
        } else {
            elementos.btnConectar.disabled = false;
            mostrarMensagemEntrada("Conexão encerrada.", true);
        }
    });

    socket.addEventListener("error", () => {
        atualizarConexao(false);
        mostrarMensagemEntrada("Erro ao conectar no servidor.", true);
        elementos.btnConectar.disabled = false;
    });
}

function tentarReconectar() {
    if (tentativaReconexao !== null) {
        return;
    }

    elementos.statusAcao.textContent = "Conexão perdida. Tentando reconectar...";

    tentativaReconexao = setTimeout(() => {
        tentativaReconexao = null;
        conectar();
    }, 1200);
}

function processarMensagem(texto) {
    let mensagem = null;

    try {
        mensagem = JSON.parse(texto);
    } catch (erro) {
        console.error("Mensagem inválida:", texto);
        return;
    }

    if (mensagem.tipo === "erro") {
        const erro = mensagem.erro || mensagem.mensagem || "Erro desconhecido.";

        if (estadoAtual === null) {
            mostrarMensagemEntrada(erro, true);
            elementos.btnConectar.disabled = false;
        } else {
            elementos.statusAcao.textContent = erro;
        }

        return;
    }

    if (mensagem.tipo === "conectado") {
        return;
    }

    if (
        mensagem.tipo === "entrada_confirmada" ||
        mensagem.tipo === "reconexao_confirmada"
    ) {
        meuId = Number(mensagem.idJogador);

        elementos.telaEntrada.classList.add("escondido");
        elementos.telaJogo.classList.remove("escondido");

        mostrarMensagemEntrada("");
        atualizarConexao(true);
        return;
    }

    if (mensagem.tipo === "estado_jogo") {
        estadoAtual = mensagem;
        meuId = Number(mensagem.meu_id);
        atualizarTela();
    }
}

function atualizarTela() {
    if (!estadoAtual) {
        return;
    }

    elementos.salaJogo.textContent = estadoAtual.sala || salaAtual;
    elementos.rodada.textContent = estadoAtual.rodada || 0;
    elementos.jogadoresConectados.textContent =
        `${estadoAtual.jogadores_conectados || 0}/${estadoAtual.max_jogadores || 0}`;

    elementos.faseJogo.textContent = nomeFase(estadoAtual.fase);
    elementos.descricaoFase.textContent = descricaoFase(estadoAtual);

    elementos.mensagemPrincipal.textContent = mensagemPrincipal(estadoAtual);
    elementos.mensagemSecundaria.textContent = mensagemSecundaria(estadoAtual);

    desenharBanca();
    desenharJogadores();
    desenharLocal();

    atualizarBotoes(true);
}

function desenharBanca() {
    const banca = estadoAtual.banca || {
        mao: [],
        pontuacao: 0
    };

    const fase = estadoAtual.fase;

    elementos.pontosBanca.textContent =
        fase === "ESCOLHENDO_ACOES" ? "?" : banca.pontuacao || 0;

    elementos.maoBanca.innerHTML = "";

    const cartas = banca.mao || [];

    if (cartas.length === 0) {
        elementos.maoBanca.innerHTML = `<div class="placeholder-cartas">Aguardando cartas</div>`;
        return;
    }

    cartas.forEach((carta) => {
        elementos.maoBanca.appendChild(criarCarta(carta));
    });
}

function desenharJogadores() {
    const jogadores = estadoAtual.jogadores || [];

    elementos.listaJogadores.innerHTML = "";

    jogadores.forEach((jogador) => {
        const card = document.createElement("div");

        card.className = "card-jogador";

        if (Number(jogador.id) === meuId) {
            card.classList.add("eu");
        }

        if (!jogador.conectado) {
            card.classList.add("desconectado");
        }

        const resultadoClasse = classeResultado(jogador.resultado);

        card.innerHTML = `
            <h3>${escapar(jogador.nome || "Jogador")}${Number(jogador.id) === meuId ? " (Você)" : ""}</h3>

            <div class="linha-status">
                <span>Status</span>
                <strong>${jogador.conectado ? "Conectado" : "Desconectado"}</strong>
            </div>

            <div class="linha-status">
                <span>Pontos</span>
                <strong>${jogador.pontuacao || 0}</strong>
            </div>

            <div class="linha-status">
                <span>Ação</span>
                <strong>${estadoJogador(jogador)}</strong>
            </div>

            <div class="mini-mao"></div>

            <div class="resultado ${resultadoClasse}">
                ${nomeResultado(jogador.resultado)}
            </div>
        `;

        const miniMao = card.querySelector(".mini-mao");

        const mao = jogador.mao || [];

        mao.forEach((carta) => {
            miniMao.appendChild(criarMiniCarta(carta));
        });

        elementos.listaJogadores.appendChild(card);
    });
}

function desenharLocal() {
    const jogador = obterJogadorLocal();

    if (!jogador) {
        elementos.nomeLocal.textContent = "Você";
        elementos.resultadoLocal.textContent = "Aguardando";
        elementos.pontosLocal.textContent = "0";
        elementos.maoLocal.innerHTML = "";
        elementos.statusAcao.textContent = "Aguardando entrada na sala.";
        return;
    }

    elementos.nomeLocal.textContent = jogador.nome || "Você";
    elementos.resultadoLocal.textContent = nomeResultado(jogador.resultado);
    elementos.pontosLocal.textContent = jogador.pontuacao || 0;

    elementos.maoLocal.innerHTML = "";

    const mao = jogador.mao || [];

    if (mao.length === 0) {
        elementos.maoLocal.innerHTML = `<div class="placeholder-cartas">Aguardando cartas</div>`;
    } else {
        mao.forEach((carta) => {
            elementos.maoLocal.appendChild(criarCarta(carta));
        });
    }

    if (!estadoAtual.partida_iniciada) {
        elementos.statusAcao.textContent = "Aguardando os jogadores entrarem.";
    } else if (estadoAtual.fase === "RESULTADO") {
        elementos.statusAcao.textContent = textoResultadoLocal(jogador.resultado);
    } else if (jogador.estourou) {
        elementos.statusAcao.textContent = "Você passou de 21 e estourou.";
    } else if (jogador.parou) {
        elementos.statusAcao.textContent = "Você parou. Aguardando os outros jogadores.";
    } else if (jogador.pode_agir) {
        elementos.statusAcao.textContent = "Sua vez: peça carta ou pare.";
    } else {
        elementos.statusAcao.textContent = "Aguardando a rodada avançar.";
    }
}

function criarCarta(carta) {
    const div = document.createElement("div");

    if (!carta || carta.oculta) {
        div.className = "carta oculta";
        return div;
    }

    div.className = "carta";

    if (cartaVermelha(carta)) {
        div.classList.add("vermelha");
    }

    const valor = valorCarta(carta.valor);
    const naipe = naipeCarta(carta.naipe);

    div.innerHTML = `
        <div class="valor-carta">${valor}</div>
        <div class="naipe-carta">${naipe}</div>
        <div class="valor-carta baixo">${valor}</div>
    `;

    return div;
}

function criarMiniCarta(carta) {
    const div = document.createElement("div");

    if (!carta || carta.oculta) {
        div.className = "mini-carta oculta";
        div.textContent = "🂠";
        return div;
    }

    div.className = "mini-carta";

    if (cartaVermelha(carta)) {
        div.classList.add("vermelha");
    }

    div.textContent = `${valorCarta(carta.valor)}${naipeCarta(carta.naipe)}`;

    return div;
}

function atualizarBotoes(conectado) {
    if (!conectado || !estadoAtual) {
        elementos.btnPedir.disabled = true;
        elementos.btnParar.disabled = true;
        elementos.btnNovaRodada.disabled = true;
        return;
    }

    elementos.btnPedir.disabled = !estadoAtual.pode_pedir;
    elementos.btnParar.disabled = !estadoAtual.pode_parar;
    elementos.btnNovaRodada.disabled = !estadoAtual.pode_iniciar_nova_rodada;
}

function pedirCarta() {
    enviar({
        tipo: "acao_jogo",
        acao: "PEDIR_CARTA"
    });
}

function parar() {
    enviar({
        tipo: "acao_jogo",
        acao: "PARAR"
    });
}

function novaRodada() {
    enviar({
        tipo: "acao_jogo",
        acao: "NOVA_RODADA"
    });
}

function enviar(objeto) {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
        return;
    }

    socket.send(JSON.stringify(objeto));
}

function atualizarConexao(online) {
    if (!elementos.pontoConexao || !elementos.textoConexao) {
        return;
    }

    elementos.pontoConexao.classList.toggle("online", online);
    elementos.textoConexao.textContent = online ? "Online" : "Desconectado";
}

function mostrarMensagemEntrada(texto, erro = false) {
    elementos.mensagemEntrada.textContent = texto;
    elementos.mensagemEntrada.className = erro
        ? "mensagem-config erro"
        : "mensagem-config sucesso";
}

function obterJogadorLocal() {
    const jogadores = estadoAtual?.jogadores || [];

    return jogadores.find((jogador) => Number(jogador.id) === meuId);
}

function obterParametroUrl(nome) {
    const params = new URLSearchParams(window.location.search);
    return params.get(nome);
}

function obterOuCriarToken(sala) {
    const chave = `blackjack_token_${sala}`;
    let token = localStorage.getItem(chave);

    if (!token) {
        token = `${Date.now()}_${Math.random().toString(36).slice(2)}_${Math.random().toString(36).slice(2)}`;
        localStorage.setItem(chave, token);
    }

    return token;
}

function nomeFase(fase) {
    switch (fase) {
        case "NAO_INICIADO":
            return "Aguardando";

        case "ESCOLHENDO_ACOES":
            return "Ações dos jogadores";

        case "TURNO_BANCA":
            return "Turno da banca";

        case "RESULTADO":
            return "Resultado";

        default:
            return "Aguardando";
    }
}

function descricaoFase(estado) {
    if (!estado.partida_iniciada) {
        return "Aguardando todos os jogadores entrarem na sala.";
    }

    switch (estado.fase) {
        case "ESCOLHENDO_ACOES":
            return "Cada jogador pode pedir carta ou parar.";

        case "TURNO_BANCA":
            return "A banca está comprando cartas.";

        case "RESULTADO":
            return "Resultado da rodada contra a banca.";

        default:
            return "Aguardando atualização da mesa.";
    }
}

function mensagemPrincipal(estado) {
    if (!estado.partida_iniciada) {
        return "Aguardando jogadores";
    }

    if (estado.fase === "ESCOLHENDO_ACOES") {
        return "Peça carta ou pare";
    }

    if (estado.fase === "TURNO_BANCA") {
        return "Banca jogando";
    }

    if (estado.fase === "RESULTADO") {
        return "Resultado da rodada";
    }

    return "Blackjack / 21";
}

function mensagemSecundaria(estado) {
    if (!estado.partida_iniciada) {
        return "A rodada começa automaticamente quando a sala estiver completa.";
    }

    if (estado.fase === "ESCOLHENDO_ACOES") {
        return "A segunda carta da banca fica oculta até o resultado.";
    }

    if (estado.fase === "TURNO_BANCA") {
        return "A banca compra cartas até atingir pelo menos 17 pontos.";
    }

    if (estado.fase === "RESULTADO") {
        return "Cada jogador é comparado individualmente contra a banca.";
    }

    return estado.mensagem || "";
}

function estadoJogador(jogador) {
    if (!jogador.conectado) {
        return "Ausente";
    }

    if (estadoAtual.fase === "RESULTADO") {
        return "Finalizado";
    }

    if (jogador.estourou) {
        return "Estourou";
    }

    if (jogador.parou) {
        return "Parou";
    }

    if (jogador.pode_agir) {
        return "Jogando";
    }

    return "Aguardando";
}

function nomeResultado(resultado) {
    switch (resultado) {
        case "VITORIA":
            return "Vitória";

        case "DERROTA":
            return "Derrota";

        case "EMPATE":
            return "Empate";

        case "ESTOUROU":
            return "Estourou";

        default:
            return "Indefinido";
    }
}

function textoResultadoLocal(resultado) {
    switch (resultado) {
        case "VITORIA":
            return "Você venceu a banca nesta rodada.";

        case "DERROTA":
            return "Você perdeu para a banca nesta rodada.";

        case "EMPATE":
            return "Você empatou com a banca.";

        case "ESTOUROU":
            return "Você passou de 21 e perdeu a rodada.";

        default:
            return "Rodada finalizada.";
    }
}

function classeResultado(resultado) {
    switch (resultado) {
        case "VITORIA":
            return "vitoria";

        case "DERROTA":
            return "derrota";

        case "EMPATE":
            return "empate";

        case "ESTOUROU":
            return "estourou";

        default:
            return "";
    }
}

function valorCarta(valor) {
    const numero = Number(valor);

    switch (numero) {
        case 1:
            return "A";

        case 11:
            return "J";

        case 12:
            return "Q";

        case 13:
            return "K";

        default:
            return String(numero || "?");
    }
}

function naipeCarta(naipe) {
    const numero = Number(naipe);

    switch (numero) {
        case 0:
            return "♣";

        case 1:
            return "♥";

        case 2:
            return "♠";

        case 3:
            return "♦";

        default:
            return "♠";
    }
}

function cartaVermelha(carta) {
    const numero = Number(carta.naipe);

    return numero === 1 || numero === 3;
}

function escapar(texto) {
    return String(texto)
        .replaceAll("&", "&amp;")
        .replaceAll("<", "&lt;")
        .replaceAll(">", "&gt;")
        .replaceAll('"', "&quot;")
        .replaceAll("'", "&#039;");
}
